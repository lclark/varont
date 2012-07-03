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
