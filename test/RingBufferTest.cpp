#include <thread>
#include <chrono>
#include <stdexcept>

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


// #include "CountDownLatch.hpp"

namespace disruptor {
namespace test {

struct StubEvent { };

struct RingBufferTest : public testing::Test {
  const int BUFFER_SIZE;
  SingleThreadedClaimStrategy claimStrategy;
  SleepingWaitStrategy waitStrategy;
  RingBuffer<StubEvent> ringBuffer;
  ProcessingSequenceBarrier sequenceBarrier;
  NoOpEventProcessor eventProcessor;

  RingBufferTest()
    : BUFFER_SIZE(32)
    , claimStrategy(BUFFER_SIZE)
    , waitStrategy()
    , ringBuffer(claimStrategy, waitStrategy)
    , sequenceBarrier(ringBuffer.newBarrier({}))
    , eventProcessor(ringBuffer)
  {
    ringBuffer.setGatingSequences({ &eventProcessor.getSequence() });
  }
};

TEST_F(RingBufferTest, twosCompliment) {
}

}
}
