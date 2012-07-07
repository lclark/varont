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
#ifndef __DISRUPTOR_RINGBUFFER_HPP__
#define __DISRUPTOR_RINGBUFFER_HPP__

#include <stdexcept>

#include "Sequencer.hpp"
#include "MultiThreadedClaimStrategy.hpp"
#include "BlockingWaitStrategy.hpp"
#include "Util.hpp"

namespace disruptor {

/**
 * Ring based store of reusable entries containing the data
 * representing an event being exchanged between event publisher and
 * {@link EventProcessor}s.
 *
 * @param <T> implementation storing the data for sharing during
 * exchange or parallel coordination of an event.
 */
template <typename T>
class RingBuffer
    : public Sequencer
{
  int indexMask_;
  T* entries_;

public:

  /**
   * Construct a RingBuffer with the full option set.
   *
   * @param claimStrategy threading strategy for publisher claiming entries in the ring.
   * @param waitStrategy waiting strategy employed by processorsToTrack waiting on entries becoming available.
   *
   * @throws IllegalArgumentException if bufferSize is not a power of 2
   */
  RingBuffer(ClaimStrategy& claimStrategy, WaitStrategy& waitStrategy)
      : Sequencer(claimStrategy, waitStrategy)
      , indexMask_(claimStrategy.getBufferSize() - 1)
      , entries_(nullptr)
  {
    if (util::bitCount(claimStrategy.getBufferSize()) != 1) {
      throw std::out_of_range("bufferSize must be a power of 2");
    }

    entries_ = new T[claimStrategy.getBufferSize()];
  }

  /**
   * Construct a RingBuffer with default strategies of:
   * {@link MultiThreadedClaimStrategy} and {@link BlockingWaitStrategy}
   *
   * @param eventFactory to newInstance entries for filling the RingBuffer
   * @param bufferSize of the RingBuffer that will be rounded up to the next power of 2
   */
  // RingBuffer(const int bufferSize)
  //     : defaultClaimStrategy_(new MultiThreadedClaimStrategy(bufferSize))
  //     , defaultWaitStrategy_(new BlockingWaitStrategy())
  //     , Sequencer(*defaultClaimStrategy_, *defaultWaitStrategy_)
  //     , indexMask_(defaultClaimStrategy_->getBufferSize() - 1)
  //     , entries_(nullptr)
  // {
  //   if (util::bitCount(defaultClaimStrategy_->getBufferSize()) != 1) {
  //     throw std::out_of_range("bufferSize must be a power of 2");
  //   }

  //   entries_ = new T[defaultClaimStrategy_->getBufferSize()];
  // }

  ~RingBuffer() {
    if (nullptr != entries_) {
      delete [] entries_;
    }
  }

  /**
   * Get the event for a given sequence in the RingBuffer.
   *
   * @param sequence for the event
   * @return event for the sequence
   */
  T& get(const long sequence) {
    return entries_[(int)sequence & indexMask_];
  }

  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
  RingBuffer(RingBuffer&&) = delete;
  RingBuffer& operator=(RingBuffer&&) = delete;
};

}

#endif /* __DISRUPTOR_RINGBUFFER_HPP__ */
