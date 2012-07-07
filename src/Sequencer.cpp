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
#include <stdexcept>

#include "Sequencer.hpp"
#include "BatchDescriptor.hpp"
#include "ProcessingSequenceBarrier.hpp"
#include "ProcessingSequenceBarrier.hpp"
#include "Util.hpp"

namespace disruptor {

void Sequencer::setGatingSequences(std::vector<Sequence*>& sequences) {
  gatingSequences_.resize(sequences.size());
  std::copy(sequences.begin(), sequences.end(), gatingSequences_.begin());
}

void Sequencer::setGatingSequences(std::vector<Sequence*>&& sequences) {
  gatingSequences_.resize(sequences.size());
  std::copy(sequences.begin(), sequences.end(), gatingSequences_.begin());
}

std::unique_ptr<ProcessingSequenceBarrier> Sequencer::newBarrier(std::vector<Sequence*>& sequencesToTrack) {
  return std::unique_ptr<ProcessingSequenceBarrier>(
    new ProcessingSequenceBarrier(waitStrategy_, cursor_, sequencesToTrack));
}

std::unique_ptr<ProcessingSequenceBarrier> Sequencer::newBarrier(std::vector<Sequence*>&& sequencesToTrack) {
  return std::unique_ptr<ProcessingSequenceBarrier>(
    new ProcessingSequenceBarrier(waitStrategy_, cursor_, sequencesToTrack));
}

BatchDescriptor Sequencer::newBatchDescriptor(const int size) {
  return BatchDescriptor(std::min(size, claimStrategy_.getBufferSize()));
}

bool Sequencer::hasAvailableCapacity(const int availableCapacity) {
  return claimStrategy_.hasAvailableCapacity(availableCapacity, gatingSequences_);
}

long Sequencer::next() {
  if (gatingSequences_.empty()) {
    throw std::out_of_range("gatingSequences must be set before claiming sequences");
  }

  return claimStrategy_.incrementAndGet(gatingSequences_);
}

long Sequencer::tryNext(const int availableCapacity) throw(InsufficientCapacityException, std::out_of_range) {
  if (gatingSequences_.empty()) {
    throw std::out_of_range("gatingSequences must be set before claiming sequences");
  }
        
  if (availableCapacity < 1) {
    throw std::out_of_range("Available capacity must be greater than 0");
  }
        
  return claimStrategy_.checkAndIncrement(availableCapacity, 1, gatingSequences_);
}

BatchDescriptor& Sequencer::next(BatchDescriptor& batchDescriptor) {
  if (gatingSequences_.empty()) {
    throw std::out_of_range("gatingSequences must be set before claiming sequences");
  }

  const long sequence = claimStrategy_.incrementAndGet(batchDescriptor.getSize(), gatingSequences_);
  batchDescriptor.setEnd(sequence);
  return batchDescriptor;
}

long Sequencer::claim(const long sequence) {
  if (gatingSequences_.empty()) {
    throw std::out_of_range("gatingSequences must be set before claiming sequences");
  }

  claimStrategy_.setSequence(sequence, gatingSequences_);
  return sequence;
}

/* Inlined: void Sequencer::publish(final long sequence) */

void Sequencer::publish(BatchDescriptor& batchDescriptor) {
  publish(batchDescriptor.getEnd(), batchDescriptor.getSize());
}

void Sequencer::forcePublish(const long sequence) {
  cursor_.set(sequence);
  waitStrategy_.signalAllWhenBlocking();
}

void Sequencer::publish(const long sequence, const int batchSize) {
  claimStrategy_.serialisePublishing(sequence, cursor_, batchSize);
  waitStrategy_.signalAllWhenBlocking();
}

const long Sequencer::remainingCapacity() {
  long consumed = util::getMinimumSequence(gatingSequences_);
  long produced = cursor_.get();
  return getBufferSize() - (produced - consumed);
}

}
