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
#ifndef __VARONT_CLAIMSTRATEGY_HPP__
#define __VARONT_CLAIMSTRATEGY_HPP__

#include <vector>

#include "InsufficientCapacityException.hpp"

namespace varont {
class Seqeunce;

/**
 * Strategy contract for claiming the sequence of events in the {@link
 * Sequencer} by event publishers.
 */
class ClaimStrategy {
public:
  /**
   * Get the size of the data structure used to buffer events.
   *
   * @return size of the underlying buffer.
   */
  virtual const int getBufferSize() const = 0;

  /**
   * Get the current claimed sequence.
   *
   * @return the current claimed sequence.
   */
  virtual long getSequence() = 0;

  /**
   * Is there available capacity in the buffer for the requested sequence.
   *
   * @param availableCapacity remaining in the buffer.
   * @param dependentSequences to be checked for range.
   * @return true if the buffer has capacity for the requested sequence.
   */
  virtual bool hasAvailableCapacity(const int availableCapacity, std::vector<Sequence*>& dependentSequences) = 0;

  /**
   * Claim the next sequence in the {@link Sequencer}.
   * The caller should be held up until the claimed sequence is available by tracking the dependentSequences.
   *
   * @param dependentSequences to be checked for range.
   * @return the index to be used for the publishing.
   */
  virtual long incrementAndGet(std::vector<Sequence*>& dependentSequences) = 0;

  /**
   * Increment sequence by a delta and get the result.
   * The caller should be held up until the claimed sequence batch is available by tracking the dependentSequences.
   *
   * @param delta to increment by.
   * @param dependentSequences to be checked for range.
   * @return the result after incrementing.
   */
  virtual long incrementAndGet(const int delta, std::vector<Sequence*>& dependentSequences) = 0;

  /**
   * Set the current sequence value for claiming an event in the {@link Sequencer}
   * The caller should be held up until the claimed sequence is available by tracking the dependentSequences.
   *
   * @param dependentSequences to be checked for range.
   * @param sequence to be set as the current value.
   */
  virtual void setSequence(const long sequence, std::vector<Sequence*>& dependentSequences) = 0;

  /**
   * Serialise publishers in sequence and set cursor to latest available sequence.
   *
   * @param sequence sequence to be applied
   * @param cursor to serialise against.
   * @param batchSize of the sequence.
   */
  virtual void serialisePublishing(const long sequence, Sequence& cursor, const int batchSize) = 0;

  /**
   * Atomically checks the available capacity of the ring buffer and claims the next sequence.  Will
   * throw InsufficientCapacityException if the capacity not available.
   * 
   * @param availableCapacity the capacity that should be available before claiming the next slot
   * @param delta the number of slots to claim
   * @param gatingSequences the set of sequences to check to ensure capacity is available
   * @return the slot after incrementing
   * @throws InsufficientCapacityException thrown if capacity is not available
   */
  virtual long checkAndIncrement(const int availableCapacity, const int delta, std::vector<Sequence*>& gatingSequences)
    throw(InsufficientCapacityException) = 0;

protected:
  ~ClaimStrategy() {}
};

}

#endif /* __VARONT_CLAIMSTRATEGY_HPP__ */
