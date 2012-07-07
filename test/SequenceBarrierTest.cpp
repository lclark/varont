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
#include <thread>
#include <chrono>
#include <stdexcept>
#include <string>
#include <future>

#include <gtest/gtest.h>

#include "Sequencer.hpp"
#include "SingleThreadedClaimStrategy.hpp"
#include "SleepingWaitStrategy.hpp"
#include "BatchDescriptor.hpp"
#include "ProcessingSequenceBarrier.hpp"
#include "InsufficientCapacityException.hpp"
#include "NoOpEventProcessor.hpp"
#include "RingBuffer.hpp"
#include "Util.hpp"

#include "BlockingWaitStrategy.hpp"
#include "MultiThreadedClaimStrategy.hpp"

#include "CountDownLatch.hpp"
#include "CyclicBarrier.hpp"

#include "support/StubEvent.hpp"

namespace disruptor {
namespace test {

class SequenceBarrierTestEventProcessor
    : public EventProcessor
{
  Sequence sequence_;
 public:
  void setSequence(Sequence sequence) { sequence_ = sequence; }
  Sequence& getSequence() { return sequence_; }

  void halt() { }
  void operator()() { }
};

class StubEventProcessor
    : public EventProcessor
{
  Sequence sequence_;
  Sequence& sequenceRef_;
 public:
  StubEventProcessor()
      : sequence_(Sequencer::INITIAL_CURSOR_VALUE)
      , sequenceRef_(sequence_)
  { }

  void setSequence(long sequence) {
    sequence_.set(sequence);
    sequenceRef_ = sequence_;
  }

  void setSequence(Sequence& sequence) {
    sequenceRef_ = sequence_;
  }

  void setSequence(Sequence&& sequence) {
    sequence_.set(sequence.get());
    sequenceRef_ = sequence_;
  }

  Sequence& getSequence() {
    return sequenceRef_;
  }

  void halt() { }
  void operator()() { }
};

class CountDownLatchSequence : public Sequence
{
  CountDownLatch& latch_;
 public:
  CountDownLatchSequence(long initialValue, CountDownLatch& latch)
      : Sequence(initialValue)
      , latch_(latch)
  { }

  long get() {
    latch_.countDown();
    return Sequence::get();
  }
};

struct SequenceBarrierTest : public testing::Test {
 public:
  MultiThreadedClaimStrategy claimStrategy;
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;

  SequenceBarrierTestEventProcessor eventProcessor1;
  SequenceBarrierTestEventProcessor eventProcessor2;
  SequenceBarrierTestEventProcessor eventProcessor3;

  NoOpEventProcessor noOpEventProcessor;

  SequenceBarrierTest()
      : claimStrategy(64)
      , waitStrategy()
      , ringBuffer(claimStrategy, waitStrategy)
      , noOpEventProcessor(ringBuffer)
  {
    ringBuffer.setGatingSequences({ &noOpEventProcessor.getSequence() });
  }

  void fillRingBuffer(long expectedNumberMessages) {
    for (long i = 0; i < expectedNumberMessages; ++i) {
      long sequence = ringBuffer.next();
      StubEvent& event = ringBuffer.get(sequence);
      event.setValue((int)i);
      ringBuffer.publish(sequence);
    }
  }
};

TEST_F(SequenceBarrierTest, shouldWaitForWorkCompleteWhereCompleteWorkThresholdIsAhead) {
  const long expectedNumberMessages = 10;
  const long expectedWorkSequence = 0;
  fillRingBuffer(expectedNumberMessages);

  const Sequence sequence1(expectedNumberMessages);
  const Sequence sequence2(expectedWorkSequence);
  const Sequence sequence3(expectedNumberMessages);

  eventProcessor1.setSequence(Sequence(expectedNumberMessages));
  eventProcessor2.setSequence(Sequence(expectedWorkSequence));
  eventProcessor3.setSequence(Sequence(expectedNumberMessages));

  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({
      &eventProcessor1.getSequence(),
      &eventProcessor2.getSequence(),
      &eventProcessor3.getSequence()
    });

  long completedWorkSequence = sequenceBarrier->waitFor(expectedWorkSequence);
  ASSERT_TRUE(completedWorkSequence >= expectedWorkSequence);
}

TEST_F(SequenceBarrierTest, shouldWaitForWorkCompleteWhereAllWorkersAreBlockedOnRingBuffer) {
  long expectedNumberMessages = 10;
  fillRingBuffer(expectedNumberMessages);

  StubEventProcessor workers[3];
  for (int i = 0, size = 3; i < size; ++i) {
    workers[i].setSequence(expectedNumberMessages - 1);
  }

  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({
      &workers[0].getSequence(),
      &workers[1].getSequence(),
      &workers[2].getSequence()
  });

  std::thread t1([&] {
      long sequence = ringBuffer.next();
      StubEvent& event = ringBuffer.get(sequence);
      event.setValue((int)sequence);
      ringBuffer.publish(sequence);

      for (auto & worker : workers) {
        worker.setSequence(sequence);
      }
    });

  t1.join();

  long expectedWorkSequence = expectedNumberMessages;
  long completedWorkSequence = sequenceBarrier->waitFor(expectedNumberMessages);
  ASSERT_TRUE(completedWorkSequence >= expectedWorkSequence);
}

TEST_F(SequenceBarrierTest, shouldInterruptDuringBusySpin) {
  long expectedNumberMessages = 10;
  fillRingBuffer(expectedNumberMessages);

  CountDownLatch latch(3);
  CountDownLatchSequence sequence1(8, latch);
  CountDownLatchSequence sequence2(8, latch);
  CountDownLatchSequence sequence3(8, latch);

  eventProcessor1.setSequence(sequence1);
  eventProcessor2.setSequence(sequence2);
  eventProcessor3.setSequence(sequence3);

  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({
      &eventProcessor1.getSequence(),
      &eventProcessor2.getSequence(),
      &eventProcessor3.getSequence()
    });

  bool alerted = false;
  std::thread t1([&] {
      try {
        sequenceBarrier->waitFor(expectedNumberMessages - 1);
      }
      catch (AlertException e) {
        alerted = true;
      }
    });

  latch.await(std::chrono::milliseconds(3000));
  sequenceBarrier->alert();
  t1.join();

  ASSERT_TRUE(alerted);
}

TEST_F(SequenceBarrierTest, shouldWaitForWorkCompleteWhereCompleteWorkThresholdIsBehind) {
  long expectedNumberMessages = 10;
  fillRingBuffer(expectedNumberMessages);

  StubEventProcessor workers[3];
  for (int i = 0, size = 3; i < size; ++i) {
    workers[i].setSequence(expectedNumberMessages - 2);
  }

  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({
      &workers[0].getSequence(),
      &workers[1].getSequence(),
      &workers[2].getSequence()
  });

  std::thread t1([&] {
      for (StubEventProcessor& worker : workers) {
        worker.setSequence(worker.getSequence().get() + 1L);
      }
    });

  long expectedWorkSequence = expectedNumberMessages - 1;
  long completedWorkSequence = sequenceBarrier->waitFor(expectedWorkSequence);
  ASSERT_TRUE(completedWorkSequence >= expectedWorkSequence);

  t1.join();
}

TEST_F(SequenceBarrierTest, shouldSetAndClearAlertStatus) {
  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({ });

  ASSERT_FALSE(sequenceBarrier->isAlerted());

  sequenceBarrier->alert();
  ASSERT_TRUE(sequenceBarrier->isAlerted());

  sequenceBarrier->clearAlert();
  ASSERT_FALSE(sequenceBarrier->isAlerted());
}


}
}
