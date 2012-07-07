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
#ifndef __VARONT_SINGLETHREADEDCLAIMSTRATEGY_HPP__
#define __VARONT_SINGLETHREADEDCLAIMSTRATEGY_HPP__

#include <thread>
#include <chrono>

#include "ClaimStrategy.hpp"
#include "Sequencer.hpp"
#include "PaddedLong.hpp"
#include "Util.hpp"

namespace varont {

/**
 * Optimised strategy can be used when there is a single publisher thread claiming sequences.
 *
 * This strategy must <b>not</b> be used when multiple threads are used for publishing concurrently on the same {@link Sequencer}
 */
class SingleThreadedClaimStrategy
  : public ClaimStrategy
{
  int bufferSize_;
  util::PaddedLong minGatingSequence_;
  util::PaddedLong claimSequence_;

public:
    /**
     * Construct a new single threaded publisher {@link ClaimStrategy} for a given buffer size.
     *
     * @param bufferSize for the underlying data structure.
     */
  SingleThreadedClaimStrategy(const int bufferSize)
    : bufferSize_(bufferSize)
    , minGatingSequence_(Sequencer::INITIAL_CURSOR_VALUE)
    , claimSequence_(Sequencer::INITIAL_CURSOR_VALUE)
  { }

  const int getBufferSize() const {
    return bufferSize_;
  }

  long getSequence() {
    return claimSequence_.get();
  }


  bool hasAvailableCapacity(const int availableCapacity, std::vector<Sequence*>& dependentSequences) {
    long wrapPoint = (claimSequence_.get() + availableCapacity) - bufferSize_;

    if (wrapPoint > minGatingSequence_.get()) {
      long minSequence = util::getMinimumSequence(dependentSequences);
      minGatingSequence_.set(minSequence);

      if (wrapPoint > minSequence) {
        return false;
      }
    }

    return true;
  }

  long incrementAndGet(std::vector<Sequence*>& dependentSequences) {
    long nextSequence = claimSequence_.get() + 1L;
    claimSequence_.set(nextSequence);
    waitForFreeSlotAt(nextSequence, dependentSequences);

    return nextSequence;
  }

  long incrementAndGet(const int delta, std::vector<Sequence*>& dependentSequences) {
    long nextSequence = claimSequence_.get() + delta;
    claimSequence_.set(nextSequence);
    waitForFreeSlotAt(nextSequence, dependentSequences);

    return nextSequence;
  }

  void setSequence(const long sequence, std::vector<Sequence*>& dependentSequences) {
    claimSequence_.set(sequence);
    waitForFreeSlotAt(sequence, dependentSequences);
  }

  void serialisePublishing(const long sequence, Sequence& cursor, const int batchSize) {
    cursor.set(sequence);
  }
    
  long checkAndIncrement(const int availableCapacity, const int delta, std::vector<Sequence*>& dependentSequences)
    throw(InsufficientCapacityException)
  {
    if (!hasAvailableCapacity(availableCapacity, dependentSequences)) {
      throw InsufficientCapacityException("");
    }

    return incrementAndGet(delta, dependentSequences);
  }

  void waitForFreeSlotAt(const long sequence, std::vector<Sequence*>& dependentSequences) {
    long wrapPoint = sequence - bufferSize_;

    if (wrapPoint > minGatingSequence_.get()) {
      long minSequence;
      while (wrapPoint > (minSequence = util::getMinimumSequence(dependentSequences))) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1L));
      }

      minGatingSequence_.set(minSequence);
    }
  }

};

}

#endif /* __VARONT_SINGLETHREADEDCLAIMSTRATEGY_HPP__ */
