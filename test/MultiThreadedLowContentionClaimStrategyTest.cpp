#include <thread>
#include <chrono>
#include <stdexcept>
#include <array>

#include <gtest/gtest.h>

#include "Sequencer.hpp"
#include "MultiThreadedLowContentionClaimStrategy.hpp"

#include "CountDownLatch.hpp"
#include "CyclicBarrier.hpp"

namespace disruptor {
namespace test {

struct MultiThreadedLowContentionClaimStrategyTest : public testing::Test {
  const int BUFFER_SIZE;

  MultiThreadedLowContentionClaimStrategy claimStrategy;

  MultiThreadedLowContentionClaimStrategyTest()
    : BUFFER_SIZE(8)
    , claimStrategy(BUFFER_SIZE)
  {}

};

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldGetCorrectBufferSize) {
  ASSERT_EQ(BUFFER_SIZE, claimStrategy.getBufferSize());
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldGetInitialSequence) {
  ASSERT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, claimStrategy.getSequence());
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldClaimInitialSequence) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + 1L;

  ASSERT_EQ(expectedSequence, claimStrategy.incrementAndGet(dependentSequences));
  ASSERT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldClaimInitialBatchOfSequences) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  long batchSize = 5;
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + batchSize;

  ASSERT_EQ(expectedSequence, claimStrategy.incrementAndGet(batchSize, dependentSequences));
  ASSERT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldSetSequenceToValue) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = 5L;

  claimStrategy.setSequence(expectedSequence, dependentSequences);

  ASSERT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldHaveInitialAvailableCapacity) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  ASSERT_TRUE(claimStrategy.hasAvailableCapacity(1L, dependentSequences));
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldNotHaveAvailableCapacityWhenBufferIsFull) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  claimStrategy.setSequence(claimStrategy.getBufferSize() - 1L, dependentSequences);

  ASSERT_FALSE(claimStrategy.hasAvailableCapacity(1L, dependentSequences));
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldNotReturnNextClaimSequenceUntilBufferHasReserve) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  claimStrategy.setSequence(claimStrategy.getBufferSize() -1L, dependentSequences);

  std::atomic_bool done(false);
  CountDownLatch beforeLatch(1);
  CountDownLatch afterLatch(1);

  std::thread publisher([&] {
      beforeLatch.countDown();

      ASSERT_EQ(claimStrategy.getBufferSize(), claimStrategy.incrementAndGet(dependentSequences));

      done = true;
      afterLatch.countDown();
    });

  beforeLatch.await();

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  EXPECT_FALSE(done);

  dependentSequence.set(dependentSequence.get() + 1L);

  afterLatch.await();
  ASSERT_EQ(claimStrategy.getBufferSize(), claimStrategy.getSequence());

  publisher.join();
}

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldSerializePublishingOnTheCursor) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  long sequence = claimStrategy.incrementAndGet(dependentSequences);

  Sequence cursor(Sequencer::INITIAL_CURSOR_VALUE);
  claimStrategy.serialisePublishing(sequence, cursor, 1);

  ASSERT_EQ(sequence, cursor.get());
}


class Sequence_ : public Sequence {
  std::atomic_bool* threadSequences_;

 public:
  Sequence_(const long initialValue, std::atomic_bool threadSequences[])
      : Sequence(initialValue)
      , threadSequences_(threadSequences)
  {}

  void set(long value) {
    threadSequences_[(int)value] = true;
    Sequence::set(value);
  }
};

TEST_F(MultiThreadedLowContentionClaimStrategyTest, shouldSerialisePublishingOnTheCursorWhenTwoThreadsArePublishing) {
  Sequence dependentSequence(Sequencer::INITIAL_CURSOR_VALUE);
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  std::atomic_bool threadSequences[2];
  threadSequences[0] = false;
  threadSequences[1] = false;

  CountDownLatch orderingLatch(1);
  Sequence_ cursor(Sequencer::INITIAL_CURSOR_VALUE, threadSequences);

  std::thread publisherOne([&] {
      long sequence = claimStrategy.incrementAndGet(dependentSequences);
      orderingLatch.countDown();

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));

      claimStrategy.serialisePublishing(sequence, cursor, 1);
    });

  std::thread publisherTwo([&] {
      orderingLatch.await();

      long sequence = claimStrategy.incrementAndGet(dependentSequences);
      claimStrategy.serialisePublishing(sequence, cursor, 1);
    });

  publisherOne.join();
  publisherTwo.join();

  ASSERT_TRUE(threadSequences[0]);
  ASSERT_TRUE(threadSequences[1]);
}

}
}
