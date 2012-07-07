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

#include <gtest/gtest.h>

#include "Sequencer.hpp"
#include "SingleThreadedClaimStrategy.hpp"
#include "CountDownLatch.hpp"

namespace varont {
namespace test {

struct SingleThreadedClaimStrategyTest : public testing::Test {
  const int BUFFER_SIZE;

  SingleThreadedClaimStrategy claimStrategy;

  SingleThreadedClaimStrategyTest()
    : BUFFER_SIZE(8)
    , claimStrategy(BUFFER_SIZE)
  {}

};

TEST_F(SingleThreadedClaimStrategyTest, shouldGetCorrectBufferSize) {
  EXPECT_EQ(BUFFER_SIZE, claimStrategy.getBufferSize());
}

TEST_F(SingleThreadedClaimStrategyTest, shouldGetInitialSequence) {
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, claimStrategy.getSequence());
}

TEST_F(SingleThreadedClaimStrategyTest, shouldClaimInitialSequence) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + 1L;

  EXPECT_EQ(expectedSequence, claimStrategy.incrementAndGet(dependentSequences));
  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(SingleThreadedClaimStrategyTest, shouldClaimInitialBatchOfSequences) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const int batchSize = 5;
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + batchSize;

  EXPECT_EQ(expectedSequence, claimStrategy.incrementAndGet(batchSize, dependentSequences));
  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(SingleThreadedClaimStrategyTest, shouldSetSequenceToValue) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  const long expectedSequence = 5;
  claimStrategy.setSequence(expectedSequence, dependentSequences);

  EXPECT_EQ(expectedSequence, claimStrategy.getSequence());
}

TEST_F(SingleThreadedClaimStrategyTest, shouldHaveInitialCapacity) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  EXPECT_TRUE(claimStrategy.hasAvailableCapacity(1, dependentSequences));
}

TEST_F(SingleThreadedClaimStrategyTest, shouldNotHaveAvailableCapacityWhenBufferIsFull) {
  Sequence dependentSequence;
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  claimStrategy.setSequence(claimStrategy.getBufferSize() - 1L, dependentSequences);

  ASSERT_FALSE(claimStrategy.hasAvailableCapacity(1, dependentSequences));
}

TEST_F(SingleThreadedClaimStrategyTest, shouldNotReturnNextClaimSequenceUntilBufferHasReserve) {
  Sequence dependentSequence(Sequencer::INITIAL_CURSOR_VALUE);
  std::vector<Sequence*> dependentSequences = { &dependentSequence };
  claimStrategy.setSequence(claimStrategy.getBufferSize() - 1L, dependentSequences);
  
  std::atomic_bool done(false);
  CountDownLatch beforeLatch(1);
  CountDownLatch afterLatch(1);

  std::thread t0([&] {
      beforeLatch.countDown();

      ASSERT_EQ(claimStrategy.getBufferSize(), claimStrategy.incrementAndGet(dependentSequences));

      done = true;
      afterLatch.countDown();
    });

  beforeLatch.await();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  ASSERT_FALSE(done.load());

  dependentSequence.set(dependentSequence.get() + 1L);

  afterLatch.await();
  ASSERT_EQ(claimStrategy.getBufferSize(), claimStrategy.getSequence());

  t0.join();
}

TEST_F(SingleThreadedClaimStrategyTest, shouldSerializePublishingOnTheCursor) {
  Sequence dependentSequence(Sequencer::INITIAL_CURSOR_VALUE);
  std::vector<Sequence*> dependentSequences = { &dependentSequence };

  long sequence = claimStrategy.incrementAndGet(dependentSequences);

  Sequence cursor(Sequencer::INITIAL_CURSOR_VALUE);
  claimStrategy.serialisePublishing(sequence, cursor, 1);

  ASSERT_EQ(sequence, cursor.get());
}

}
}
