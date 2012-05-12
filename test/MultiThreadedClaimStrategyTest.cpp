#include <thread>
#include <chrono>
#include <stdexcept>
#include <array>

#include <gtest/gtest.h>

#include "Sequencer.hpp"
#include "MultiThreadedClaimStrategy.hpp"

#include "CountDownLatch.hpp"
#include "CyclicBarrier.hpp"

namespace disruptor {
namespace test {

struct MultiThreadedClaimStrategyTest : public testing::Test {
  const int BUFFER_SIZE;

  MultiThreadedClaimStrategy claimStrategy;

  MultiThreadedClaimStrategyTest()
    : BUFFER_SIZE(8)
    , claimStrategy(BUFFER_SIZE)
  {}

};

TEST_F(MultiThreadedClaimStrategyTest, shouldNotCreateBufferWithNonPowerOf2) {
  EXPECT_THROW(new MultiThreadedClaimStrategy(1024, 129), std::out_of_range);
}

TEST_F(MultiThreadedClaimStrategyTest, shouldGetCorrectBufferSize) {
  EXPECT_EQ(BUFFER_SIZE, claimStrategy.getBufferSize());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldGetInitialSequence) {
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, claimStrategy.getSequence());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldClaimInitialSequence) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + 1L;

  EXPECT_EQ(expectedSequence, claimStrategy.incrementAndGet(dependentSequences));
  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldClaimInitialBatchOfSequences) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const int batchSize = 5;
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + batchSize;

  EXPECT_EQ(expectedSequence, claimStrategy.incrementAndGet(batchSize, dependentSequences));
  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldSetSequenceToValue) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = 5;
  claimStrategy.setSequence(expectedSequence, dependentSequences);

  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldHaveInitialCapacity) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  EXPECT_TRUE(claimStrategy.hasAvailableCapacity(1, dependentSequences));
}

TEST_F(MultiThreadedClaimStrategyTest, shouldNotHaveAvailableCapacityWhenBufferIsFull) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  claimStrategy.setSequence(claimStrategy.getBufferSize() - 1L, dependentSequences);

  EXPECT_FALSE(claimStrategy.hasAvailableCapacity(1, dependentSequences));
}

TEST_F(MultiThreadedClaimStrategyTest, shouldNotReturnNextClaimSequenceUntilBufferHasReserve) {
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

TEST_F(MultiThreadedClaimStrategyTest, shouldSerializePublishingOnTheCursor) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  long sequence = claimStrategy.incrementAndGet(dependentSequences);

  Sequence cursor(Sequencer::INITIAL_CURSOR_VALUE);
  claimStrategy.serialisePublishing(sequence, cursor, 1);

  ASSERT_EQ(sequence, cursor.get());
}

TEST_F(MultiThreadedClaimStrategyTest, shouldThrowExceptionIfCapacityIsNotAvailable) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  EXPECT_THROW(claimStrategy.checkAndIncrement(9, 1, dependentSequences), InsufficientCapacityException);
}

TEST_F(MultiThreadedClaimStrategyTest, shouldSuccessfullyGetNextValueIfLessThanCapacityIsAvailable) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  for (long i = 0; i < 8; ++i) {
    ASSERT_EQ(claimStrategy.checkAndIncrement(1, 1, dependentSequences), i);
  }
}

TEST_F(MultiThreadedClaimStrategyTest, shouldSuccessfullyGetNextValueIfLessThanCapacityIsAvailableWhenClaimingMoreThanOne) { // :-)
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  ASSERT_EQ(3L, claimStrategy.checkAndIncrement(4, 4, dependentSequences));
  ASSERT_EQ(7L, claimStrategy.checkAndIncrement(4, 4, dependentSequences));
}

class ClaimRunnable {
  MultiThreadedClaimStrategy& claimStrategy_;
  CyclicBarrier& barrier_;
  std::atomic_long* claimed_;
  std::vector<Sequence*>& dependentSequences_;

public:
  ClaimRunnable(MultiThreadedClaimStrategy& claimStrategy, CyclicBarrier& barrier, std::atomic_long claimed[], std::vector<Sequence*>& dependentSequences)
    : claimStrategy_(claimStrategy)
    , barrier_(barrier)
    , claimed_(claimed)
    , dependentSequences_(dependentSequences)
  {}

  void operator()() {
    try {
      barrier_.await();
      long next = claimStrategy_.checkAndIncrement(1, 1, dependentSequences_);
      claimed_[(int)next].fetch_add(1);
    }
    catch (...) {
    }
    
  }
};

TEST_F(MultiThreadedClaimStrategyTest, shouldOnlyClaimWhatsAvailable) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  for (int j = 0; j < 10000; ++j) {
    int numThreads = BUFFER_SIZE * 2;
    MultiThreadedClaimStrategy claimStrategy(BUFFER_SIZE);
    CyclicBarrier barrier(numThreads);
    std::atomic_long claimed[numThreads];
    std::thread ts[numThreads];

    for (int i = 0; i < numThreads; i++) {
      claimed[i] = 0;
    }

    for (int i = 0; i < numThreads; i++) {
      ts[i] = std::thread(ClaimRunnable(claimStrategy, barrier, claimed, dependentSequences));
    }

    for (int i = 0; i < numThreads; i++) {
      ts[i].join();
    }

    for (int i = 0; i < BUFFER_SIZE; ++i) {
      ASSERT_EQ(claimed[i], 1L);
    }

    for (int i = BUFFER_SIZE; i < numThreads; ++i) {
      ASSERT_EQ(claimed[i], 0L);
    }
  }
}


}
}
