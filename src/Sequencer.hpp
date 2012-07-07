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
#ifndef __DISRUPTOR_SEQUENCER_HPP__
#define __DISRUPTOR_SEQUENCER_HPP__

#include <stdexcept>
#include <memory>

#include "Sequence.hpp"
#include "ClaimStrategy.hpp"
#include "WaitStrategy.hpp"

namespace disruptor {
class BatchDescriptor;
class ProcessingSequenceBarrier;

/**
 * Coordinator for claiming sequences for access to a data strcuture
 * while trading dependent Sequences.
 */
class Sequencer {
  Sequence cursor_;
  std::vector<Sequence*> gatingSequences_;

  ClaimStrategy& claimStrategy_;
  WaitStrategy& waitStrategy_;
public:
  static const long INITIAL_CURSOR_VALUE = -1L;

  /**
   * Construct a Sequencer with the selected strategies.
   *
   * @param claimStrategy for those claiming sequences.
   * @param waitStrategy for those waiting on the sequences.
   */
  Sequencer(ClaimStrategy& claimStrategy, WaitStrategy& waitStrategy)
    : cursor_(INITIAL_CURSOR_VALUE)
    , claimStrategy_(claimStrategy)
    , waitStrategy_(waitStrategy)
  {}

  /**
   * Set the sequences that will gate publishers to prevent the buffer wrapping.
   *
   * XXX: This method must be called prior to claiming sequences
   * otherwise a NullPointerException will be thrown.
   *
   * @param sequences to be to be gated on.
   */
  void setGatingSequences(std::vector<Sequence*>& sequences);

  void setGatingSequences(std::vector<Sequence*>&& sequences);

  /**
   * Create a {@link SequenceBarrier} that gates on the the cursor and a list of {@link Sequence}s
   *
   * @param sequencesToTrack this barrier will track
   * @return the barrier gated as required
   */
  std::unique_ptr<ProcessingSequenceBarrier> newBarrier(std::vector<Sequence*>& sequencesToTrack);

  std::unique_ptr<ProcessingSequenceBarrier> newBarrier(std::vector<Sequence*>&& sequencesToTrack);

  /**
   * Create a new {@link BatchDescriptor} that is the minimum of the requested size
   * and the buffer size.
   *
   * @param size for the batch
   * @return the new {@link BatchDescriptor}
   */
  BatchDescriptor newBatchDescriptor(const int size);

  /**
   * The capacity of the data structure to hold entries.
   *
   * @return the size of the RingBuffer.
   */
  const int getBufferSize() {
    return claimStrategy_.getBufferSize();
  }

  /**
   * Get the value of the cursor indicating the published sequence.
   *
   * @return value of the cursor for events that have been published.
   */
  long getCursor() {
    return cursor_.get();
  }

  /**
   * Has the buffer got capacity to allocate another sequence.  This is a concurrent
   * method so the response should only be taken as an indication of available capacity.
   *
   * @param availableCapacity in the buffer
   * @return true if the buffer has the capacity to allocate the next sequence otherwise false.
   */
  bool hasAvailableCapacity(const int availableCapacity);

  /**
   * Claim the next event in sequence for publishing.
   *
   * @return the claimed sequence value
   */
  long next();

  /**
   * Attempt to claim the next event in sequence for publishing.  Will return the
   * number of the slot if there is at least <code>availableCapacity</code> slots
   * available.  
   * 
   * @param availableCapacity
   * @return the claimed sequence value
   * @throws InsufficientCapacityException
   */
  long tryNext(const int availableCapacity) throw(InsufficientCapacityException, std::out_of_range);

  /**
   * Claim the next batch of sequence numbers for publishing.
   *
   * @param batchDescriptor to be updated for the batch range.
   * @return the updated batchDescriptor.
   */
  BatchDescriptor& next(BatchDescriptor& batchDescriptor);

  /**
   * Claim a specific sequence when only one publisher is involved.
   *
   * @param sequence to be claimed.
   * @return sequence just claimed.
   */
  long claim(const long sequence);

  /**
   * Publish an event and make it visible to {@link EventProcessor}s
   *
   * @param sequence to be published
   */
  void publish(const long sequence) {
    publish(sequence, 1);
  }

  /**
   * Publish the batch of events in sequence.
   *
   * @param batchDescriptor to be published.
   */
  void publish(BatchDescriptor& batchDescriptor);

  /**
   * Force the publication of a cursor sequence.
   *
   * Only use this method when forcing a sequence and you are sure only one publisher exists.
   * This will cause the cursor to advance to this sequence.
   *
   * @param sequence which is to be forced for publication.
   */
  void forcePublish(const long sequence);

  void publish(const long sequence, const int batchSize);

  const long remainingCapacity();

};

}

#endif /* __DISRUPTOR_SEQUENCER_HPP__ */
