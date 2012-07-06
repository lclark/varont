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

struct BatchPublisherTest : public testing::Test {
  const int BUFFER_SIZE;
  MultiThreadedClaimStrategy claimStrategy;
  BlockingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;
  std::unique_ptr<ProcessingSequenceBarrier> sequenceBarrier;
  NoOpEventProcessor noOpEventProcessor;

  BatchPublisherTest()
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

TEST_F(BatchPublisherTest, shouldClaimBatchAndPublishBack) {
  int batchSize = 5;
  BatchDescriptor batchDescriptor = ringBuffer.newBatchDescriptor(batchSize);

  ringBuffer.next(batchDescriptor);

  ASSERT_EQ(0L, batchDescriptor.getStart());
  ASSERT_EQ(4L, batchDescriptor.getEnd());
  ASSERT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, ringBuffer.getCursor());

  ringBuffer.publish(batchDescriptor);

  ASSERT_EQ(batchSize - 1L, ringBuffer.getCursor());
  ASSERT_EQ(batchSize - 1L, sequenceBarrier->waitFor(0L));
}

}
}
