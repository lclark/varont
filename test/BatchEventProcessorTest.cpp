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
#include "BatchEventProcessor.hpp"
#include "RingBuffer.hpp"
#include "Util.hpp"

#include "BlockingWaitStrategy.hpp"
#include "MultiThreadedClaimStrategy.hpp"

#include "CountDownLatch.hpp"
#include "CyclicBarrier.hpp"

#include "support/StubEvent.hpp"

namespace disruptor {
namespace test {

class StubEventHandler
    : public LifecycleAwareEventHandler<StubEvent>
{
  CountDownLatch& latch_;
 public:
  StubEventHandler(CountDownLatch& latch)
      : latch_(latch)
  { }

  void onEvent(StubEvent& event, long sequence, bool endOfBatch) {
    if (endOfBatch) {
      latch_.countDown();
    }
  }

  void onStart() { }
  void onShutdown() { }
};

struct BatchEventProcessorTest : public testing::Test {
 public:
  CountDownLatch latch;
  MultiThreadedClaimStrategy claimStrategy;
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;
  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier;
  StubEventHandler eventHandler;

  BatchEventProcessorTest()
      : latch(1)
      , claimStrategy(16)
      , waitStrategy()
      , ringBuffer(claimStrategy, waitStrategy)
      , sequenceBarrier(ringBuffer.newBarrier({ }))
      , eventHandler(latch)
  { }
};

TEST_F(BatchEventProcessorTest, shouldCallMethodsInLifecycleOrder) {
  BatchEventProcessor<StubEvent> batchEventProcessor(ringBuffer, *sequenceBarrier.get(), eventHandler);
  ringBuffer.setGatingSequences({ &batchEventProcessor.getSequence() });

  std::thread t1(std::ref(batchEventProcessor));

  ASSERT_EQ(-1L, batchEventProcessor.getSequence().get());
  ringBuffer.publish(ringBuffer.next());
  latch.await();
  batchEventProcessor.halt();

  t1.join();
}

TEST_F(BatchEventProcessorTest, shouldCallMethodsInLifecycleOrderForBatch) {
  BatchEventProcessor<StubEvent> batchEventProcessor(ringBuffer, *sequenceBarrier.get(), eventHandler);
  ringBuffer.setGatingSequences({ &batchEventProcessor.getSequence() });

  ringBuffer.publish(ringBuffer.next());
  ringBuffer.publish(ringBuffer.next());
  ringBuffer.publish(ringBuffer.next());

  std::thread t1(std::ref(batchEventProcessor));

  latch.await();
  batchEventProcessor.halt();

  t1.join();
}

TEST_F(BatchEventProcessorTest, shouldCallExceptionHandlerOnUncaughtException) {
  class PregnantExceptionHandler : public ExceptionHandler {
    CountDownLatch& latch_;
   public:
    PregnantExceptionHandler(CountDownLatch& latch)
        : latch_(latch)
    { }

    void handleEventException(std::exception& ex, long sequence) {
      latch_.countDown();
    }
    void handleOnStartException(std::exception& ex) { }
    void handleOnShutdownException(std::exception& ex) { }
  };

  class StubEventHandlerFatale
      : public LifecycleAwareEventHandler<StubEvent>
  {
   public:
    StubEventHandlerFatale() { }

    void onEvent(StubEvent& event, long sequence, bool endOfBatch) {
      throw std::exception();
    }

    void onStart() { }
    void onShutdown() { }
  };

  PregnantExceptionHandler exceptionHandler(latch);
  StubEventHandlerFatale eventHandler_;

  BatchEventProcessor<StubEvent> batchEventProcessor(ringBuffer, *sequenceBarrier.get(), eventHandler_);
  batchEventProcessor.setExceptionHandler(exceptionHandler);
  ringBuffer.setGatingSequences({ &batchEventProcessor.getSequence() });

  std::thread t1(std::ref(batchEventProcessor));
  
  ringBuffer.publish(ringBuffer.next());

  ASSERT_TRUE(latch.await(std::chrono::milliseconds(3000)));

  batchEventProcessor.halt();
  t1.join();
}

}
}
