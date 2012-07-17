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

namespace varont {
namespace test {

struct RingBufferTest : public testing::Test {
  const int BUFFER_SIZE;
  MultiThreadedClaimStrategy claimStrategy;
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;
  std::unique_ptr<SequenceBarrier> sequenceBarrier;
  NoOpEventProcessor noOpEventProcessor;

  RingBufferTest()
    : BUFFER_SIZE(32)
    , claimStrategy(BUFFER_SIZE)
    , waitStrategy()
    , ringBuffer(claimStrategy, waitStrategy)
    , sequenceBarrier(ringBuffer.newBarrier({}))
    , noOpEventProcessor(ringBuffer)
  {
    ringBuffer.setGatingSequences({ &noOpEventProcessor.getSequence() });
  }
};

TEST_F(RingBufferTest, shouldClaimAndGet) {
  ASSERT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, ringBuffer.getCursor());

  StubEvent expectedEvent(2701);

  long claimSequence = ringBuffer.next();
  StubEvent& oldEvent = ringBuffer.get(claimSequence);
  oldEvent = expectedEvent;
  ringBuffer.publish(claimSequence);

  long sequence = sequenceBarrier->waitFor(0);
  ASSERT_EQ(0, sequence);

  StubEvent& event = ringBuffer.get(sequence);
  ASSERT_EQ(expectedEvent, event);

  ASSERT_EQ(0L, ringBuffer.getCursor());
}

TEST_F(RingBufferTest, shouldClaimAndGetWithTimeout) {
  ASSERT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, ringBuffer.getCursor());

  StubEvent expectedEvent(2701);

  long claimSequence = ringBuffer.next();
  StubEvent& oldEvent = ringBuffer.get(claimSequence);
  oldEvent = expectedEvent;
  ringBuffer.publish(claimSequence);

  long sequence = sequenceBarrier->waitFor(0, 5, TimeUnit::Milliseconds);
  ASSERT_EQ(0, sequence);

  StubEvent& event(ringBuffer.get(sequence));
  ASSERT_EQ(expectedEvent, event);

  ASSERT_EQ(0L, ringBuffer.getCursor());
}

TEST_F(RingBufferTest, shouldGetWithTimeout) {
  long sequence = sequenceBarrier->waitFor(0, 5, TimeUnit::Milliseconds);
  ASSERT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, sequence);
}

TEST_F(RingBufferTest, shouldClaimAndGetWithSeparateThread) {
  /* Publish a message to the buffer and verify that a waiting thread
   * receives it. */
  CyclicBarrier cyclicBarrier(2);
  const long initialSequence = 0;
  const long toWaitForSequence = 0;
  std::vector<StubEvent> messages;

  std::unique_ptr<SequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({});

  StubEvent expectedEvent(2701);

  const long sequence = ringBuffer.next();
  StubEvent& oldEvent = ringBuffer.get(sequence);
  oldEvent = expectedEvent;
  ringBuffer.publish(sequence);

  auto f = std::async(std::launch::async, [&] {
      cyclicBarrier.await();
      sequenceBarrier->waitFor(toWaitForSequence);

      for (long l = initialSequence; l <= toWaitForSequence; ++l) {
        messages.push_back(ringBuffer.get(l));
      }

      return true;
    });

  cyclicBarrier.await();

  ASSERT_TRUE(f.get());
  ASSERT_EQ(expectedEvent, messages.at(0));
}

TEST_F(RingBufferTest, shouldClaimAndGetMultipleMessages) {
  int numMessages = ringBuffer.getBufferSize();
  for (int i = 0; i < numMessages; ++i) {
    long sequence = ringBuffer.next();
    StubEvent& event = ringBuffer.get(sequence);
    event.setValue(i);
    ringBuffer.publish(sequence);
  }

  int expectedSequence = numMessages - 1;
  long available = sequenceBarrier->waitFor(expectedSequence);
  ASSERT_EQ(expectedSequence, available);

  for (int i = 0; i < numMessages; ++i) {
    ASSERT_EQ(i, ringBuffer.get(i).get());
  }
}

TEST_F(RingBufferTest, shouldWrap) {
  int numMessages = ringBuffer.getBufferSize();
  int offset = 1000;
  for (int i = 0; i < numMessages + offset; i++) {
    long sequence = ringBuffer.next();
    StubEvent& event = ringBuffer.get(sequence);
    event.setValue(i);
    ringBuffer.publish(sequence);
  }

  int expectedSequence = numMessages + offset - 1;
  long available = sequenceBarrier->waitFor(expectedSequence);
  ASSERT_EQ(expectedSequence, available);

  for (int i = offset; i < numMessages + offset; i++) {
    ASSERT_EQ(i, ringBuffer.get(i).get());
  }
}

TEST_F(RingBufferTest, shouldSetAtSpecificSequence) {
  long expectedSequence = 5;

  ringBuffer.claim(expectedSequence);
  StubEvent& expectedEvent = ringBuffer.get(expectedSequence);
  expectedEvent.setValue((int) expectedSequence);
  ringBuffer.forcePublish(expectedSequence);

  long sequence = sequenceBarrier->waitFor(expectedSequence);
  ASSERT_EQ(expectedSequence, sequence);

  StubEvent& event = ringBuffer.get(sequence);
  ASSERT_EQ(expectedEvent, event);

  ASSERT_EQ(expectedSequence, ringBuffer.getCursor());
}

class TestEventProcessor
    : public EventProcessor
{
 private:
  SequenceBarrier& sequenceBarrier_;
  Sequence sequence_;

 public:
  TestEventProcessor(SequenceBarrier& sequenceBarrier)
      : sequenceBarrier_(sequenceBarrier)
      , sequence_(Sequencer::INITIAL_CURSOR_VALUE)
  {}

  Sequence& getSequence() {
    return const_cast<Sequence&>(sequence_);
  }

  void halt() {}

  void operator()() {
    sequenceBarrier_.waitFor(0L);

    sequence_.set(sequence_.get() + 1L);
  }
};


TEST_F(RingBufferTest, shouldPreventPublishersOvertakingEventProcessorWrapPoint) {
  const int ringBufferSize = 4;
  CountDownLatch latch(ringBufferSize);
  std::atomic_bool publisherComplete(false);
  MultiThreadedClaimStrategy claimStrategy(ringBufferSize);
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer(claimStrategy, waitStrategy);

  std::unique_ptr<SequenceBarrier> sequenceBarrier = ringBuffer.newBarrier({});

  TestEventProcessor processor(*sequenceBarrier);
  ringBuffer.setGatingSequences({ &processor.getSequence() });
  std::thread thread([&] {
      for (int i = 0; i <= ringBufferSize; i++) {
        long sequence = ringBuffer.next();
        StubEvent& event = ringBuffer.get(sequence);
        event.setValue(i);
        ringBuffer.publish(sequence);
        latch.countDown();
      }

      publisherComplete.store(true);
    });

  latch.await();
  EXPECT_EQ((long)ringBuffer.getCursor(), (long)(ringBufferSize - 1));
  ASSERT_FALSE(publisherComplete.load());

  processor();
  thread.join();

  ASSERT_TRUE(publisherComplete.load());
}

}
}
