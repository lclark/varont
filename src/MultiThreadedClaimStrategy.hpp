/*
 * Copyright 2012 Leonard Clark
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __DISRUPTOR_MULTITHREADEDCLAIMSTRATEGY_HPP__
#define __DISRUPTOR_MULTITHREADEDCLAIMSTRATEGY_HPP__

#include <stdexcept>

#include "AbstractMultithreadedClaimStrategy.hpp"

namespace disruptor {

class MultiThreadedClaimStrategy
    : public AbstractMultithreadedClaimStrategy
{
  static const int RETRIES = 1000;

  std::atomic_long* pendingPublication_;
  std::size_t pendingBufferSize_;
  int pendingMask_;

public:
  /**
   * Construct a new multi-threaded publisher {@link ClaimStrategy} for a given buffer size.
   *
   * @param bufferSize for the underlying data structure.
   * @param pendingBufferSize number of item that can be pending for serialisation
   */
  MultiThreadedClaimStrategy(const int bufferSize, const int pendingBufferSize)
    : AbstractMultithreadedClaimStrategy(bufferSize)
    , pendingPublication_(nullptr)
    , pendingBufferSize_(pendingBufferSize)
    , pendingMask_(pendingBufferSize - 1)
  {
    if (util::bitCount(pendingBufferSize) != 1) {
      throw std::out_of_range("pendingBufferSize must be a power of 2, was: " + pendingBufferSize_);
    }

    pendingPublication_ = new std::atomic_long[pendingBufferSize_];
  }

  /**
   * Construct a new multi-threaded publisher {@link ClaimStrategy} for a given buffer size.
   *
   * @param bufferSize for the underlying data structure.
   */
  MultiThreadedClaimStrategy(const int bufferSize)
    : AbstractMultithreadedClaimStrategy(bufferSize)
    , pendingPublication_(nullptr)
    , pendingBufferSize_(1024)
    , pendingMask_(pendingBufferSize_ - 1)
  {
    pendingPublication_ = new std::atomic_long[pendingBufferSize_];
  }

  ~MultiThreadedClaimStrategy() {
    if (nullptr != pendingPublication_) {
      delete [] pendingPublication_;
    }
  }

  MultiThreadedClaimStrategy(const MultiThreadedClaimStrategy&) = delete;
  MultiThreadedClaimStrategy& operator=(const MultiThreadedClaimStrategy&) = delete;

  void serialisePublishing(const long sequence, Sequence& cursor, const int batchSize) {
    int counter = RETRIES;
    while ((sequence - cursor.get()) > (int)pendingBufferSize_) {
      if (--counter == 0) {
        std::this_thread::yield();
        counter = RETRIES;
      }
    }

    long expectedSequence = sequence - batchSize;
    for (long pendingSequence = expectedSequence + 1; pendingSequence < sequence; pendingSequence++) {
      /* XXX: I *believe* store(..., std::memory_order_relaxed) to be
         the correct alternative to lazySet.  Whether that is the
         reality, we shall see. */
      pendingPublication_[(int) pendingSequence & pendingMask_].store(pendingSequence, std::memory_order_relaxed);
    }
    pendingPublication_[(int) sequence & pendingMask_].store(sequence);

    long cursorSequence = cursor.get();
    if (cursorSequence >= sequence) {
      return;
    }

    expectedSequence = std::max(expectedSequence, cursorSequence);
    long nextSequence = expectedSequence + 1;
    while (cursor.compareAndSet(expectedSequence, nextSequence)) {
      expectedSequence = nextSequence;
      nextSequence++;
      if (pendingPublication_[(int) nextSequence & pendingMask_] != nextSequence) {
        break;
      }
    }
  }

};

}

#endif /* __DISRUPTOR_MULTITHREADEDCLAIMSTRATEGY_HPP__ */
