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
#include <memory>

#include <gtest/gtest.h>

#include "CountDownLatch.hpp"
#include "LifecycleAwareEventHandler.hpp"
#include "RingBuffer.hpp"
#include "NoOpEventProcessor.hpp"
#include "MultiThreadedClaimStrategy.hpp"
#include "ProcessingSequenceBarrier.hpp"
#include "BatchEventProcessor.hpp"

#include "support/StubEvent.hpp"

namespace disruptor {
namespace test {


class LifecycleAwareStubEventHandler
    : public LifecycleAwareEventHandler<StubEvent>
{
  int startCounter_;
  int shutdownCounter_;

  CountDownLatch& startLatch_;
  CountDownLatch& shutdownLatch_;

 public:
  LifecycleAwareStubEventHandler(CountDownLatch& startLatch, CountDownLatch& shutdownLatch)
      : startCounter_(0)
      , shutdownCounter_(0)
      , startLatch_(startLatch)
      , shutdownLatch_(shutdownLatch)
  {}

  void onEvent(StubEvent& event, long sequence, bool endOfBatch) {
    printf("meh\n");
  }

  void onStart() {
    ++startCounter_;
    startLatch_.countDown();
  }

  void onShutdown() {
    ++shutdownCounter_;
    shutdownLatch_.countDown();
  }

  int startCounter() const { return startCounter_; }
  int shutdownCounter() const { return shutdownCounter_; }
};

struct LifecycleAwareTest : public testing::Test {
  CountDownLatch startLatch;
  CountDownLatch shutdownLatch;

  const int BUFFER_SIZE;
  MultiThreadedClaimStrategy claimStrategy;
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;
  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier;
  LifecycleAwareStubEventHandler handler;
  BatchEventProcessor<StubEvent> batchEventProcessor;

  LifecycleAwareTest()
      : startLatch(1)
      , shutdownLatch(1)

      , BUFFER_SIZE(16)
      , claimStrategy(BUFFER_SIZE)
      , waitStrategy()
      , ringBuffer(claimStrategy, waitStrategy)
      , sequenceBarrier(ringBuffer.newBarrier({}))
      , handler(startLatch, shutdownLatch)
      , batchEventProcessor(ringBuffer, *sequenceBarrier.get(), handler)
      
  {
  }
};

TEST_F(LifecycleAwareTest, shouldNotifyOfBatchProcessorLifecycle) {
  std::thread batchEventProcessorThread(std::ref(batchEventProcessor));

  startLatch.await();
  batchEventProcessor.halt();

  shutdownLatch.await();

  ASSERT_EQ(1, handler.startCounter());
  ASSERT_EQ(1, handler.shutdownCounter());

  batchEventProcessorThread.join();
}

}
}
