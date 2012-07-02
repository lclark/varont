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

#include "CountDownLatch.hpp"

namespace disruptor {
namespace test {

struct SequencerTest : public testing::Test {
  const int BUFFER_SIZE;
  Sequence gatingSequence;

  SingleThreadedClaimStrategy claimStrategy_;
  SleepingWaitStrategy waitStrategy_;
  Sequencer sequencer;

  SequencerTest()
    : BUFFER_SIZE(4)
    , gatingSequence(Sequencer::INITIAL_CURSOR_VALUE)
    , claimStrategy_(BUFFER_SIZE)
    , waitStrategy_()

    , sequencer(claimStrategy_, waitStrategy_)
  {
    sequencer.setGatingSequences({&gatingSequence});
  }

  void fillBuffer() {
    for (int i = 0; i < BUFFER_SIZE; ++i) {
      long sequence = sequencer.next();
      sequencer.publish(sequence);
    }
  }
};

TEST_F(SequencerTest, shouldStartWithInitialValue) {
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, sequencer.getCursor());
}

TEST_F(SequencerTest, shouldGetPublishFirstSequence) {
  long sequence = sequencer.next();
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, sequencer.getCursor());
  EXPECT_EQ(0L, sequence);

  sequencer.publish(sequence);
  EXPECT_EQ(sequence, sequencer.getCursor());
}


TEST_F(SequencerTest, shouldIndicateAvailableCapacity) {
  EXPECT_TRUE(sequencer.hasAvailableCapacity(1));
}

TEST_F(SequencerTest, voidShouldIndicateNoAvailableCapacity) {
  fillBuffer();
  EXPECT_FALSE(sequencer.hasAvailableCapacity(1));
}

TEST_F(SequencerTest, shouldForceClaimSequence) {
  long claimSequence = 3L;

  long sequence = sequencer.claim(claimSequence);
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, sequencer.getCursor());
  EXPECT_EQ(sequence, claimSequence);

  sequencer.forcePublish(sequence);
  EXPECT_EQ(claimSequence, sequencer.getCursor());
}

TEST_F(SequencerTest, shouldPublishSequenceBatch) {
  const int batchSize = 3;
  BatchDescriptor batchDescriptor(batchSize);

  sequencer.next(batchDescriptor);
  EXPECT_EQ((long)Sequencer::INITIAL_CURSOR_VALUE, sequencer.getCursor());
  EXPECT_EQ(batchDescriptor.getEnd(), (long)Sequencer::INITIAL_CURSOR_VALUE + batchSize);
  EXPECT_EQ(batchDescriptor.getSize(), batchSize);

  sequencer.publish(batchDescriptor);
  EXPECT_EQ(sequencer.getCursor(), (long)Sequencer::INITIAL_CURSOR_VALUE + batchSize);
}

TEST_F(SequencerTest, shouldAwaitOnSequence) {
  std::unique_ptr<ProcessingSequenceBarrier> barrier = sequencer.newBarrier({});
  long sequence = sequencer.next();
  sequencer.publish(sequence);

  EXPECT_EQ(sequence, barrier->waitFor(sequence));
}

TEST_F(SequencerTest, shouldWaitOnSequenceShowingBatchingEffect) {
  std::unique_ptr<ProcessingSequenceBarrier> barrier = sequencer.newBarrier({});
  sequencer.publish(sequencer.next());
  sequencer.publish(sequencer.next());

  long sequence = sequencer.next();
  sequencer.publish(sequence);

  EXPECT_EQ(sequence, barrier->waitFor((long)Sequencer::INITIAL_CURSOR_VALUE + 1L));
}

TEST_F(SequencerTest, shouldSignalWaitingProcessorWhenSequenceIsPublished) {
  std::unique_ptr<ProcessingSequenceBarrier> barrier = sequencer.newBarrier({});
  CountDownLatch waitingLatch(1);
  CountDownLatch doneLatch(1);
  const long expectedSequence = Sequencer::INITIAL_CURSOR_VALUE + 1L;

  std::thread t0([&] {
      waitingLatch.countDown();
      EXPECT_EQ(expectedSequence, barrier->waitFor(expectedSequence));

      gatingSequence.set(expectedSequence);
      doneLatch.countDown();
    });

  waitingLatch.await();
  EXPECT_EQ(gatingSequence.get(), (long)Sequencer::INITIAL_CURSOR_VALUE);

  sequencer.publish(sequencer.next());

  doneLatch.await();
  EXPECT_EQ(gatingSequence.get(), expectedSequence);

  t0.join();
}

TEST_F(SequencerTest, shouldHoldUpPublisherWhenBufferIsFull) {
  fillBuffer();

  CountDownLatch waitingLatch(1);
  CountDownLatch doneLatch(1);

  long expectedFullSequence = Sequencer::INITIAL_CURSOR_VALUE + sequencer.getBufferSize();
  EXPECT_EQ(sequencer.getCursor(), expectedFullSequence);

  std::thread t0([&] {
      waitingLatch.countDown();
      sequencer.publish(sequencer.next());
      doneLatch.countDown();
    });

  waitingLatch.await();
  EXPECT_EQ(sequencer.getCursor(), expectedFullSequence);

  gatingSequence.set(Sequencer::INITIAL_CURSOR_VALUE + 1L);

  doneLatch.await();
  EXPECT_EQ(sequencer.getCursor(), expectedFullSequence + 1L);

  t0.join();
}

TEST_F(SequencerTest, shouldThrowInsufficientCapacityExceptionWhenSequencerIsFull) {
  EXPECT_THROW(sequencer.tryNext(5), InsufficientCapacityException);
}

TEST_F(SequencerTest, shouldRejectAvailableCapacityLessThanOne) {
  EXPECT_THROW(sequencer.tryNext(0), std::out_of_range);
}

TEST_F(SequencerTest, shouldCalculateRemainingCapacity) {
  EXPECT_EQ(4L, sequencer.remainingCapacity());
  sequencer.publish(sequencer.next());
  EXPECT_EQ(3L, sequencer.remainingCapacity());
  sequencer.publish(sequencer.next());
  EXPECT_EQ(2L, sequencer.remainingCapacity());
  sequencer.publish(sequencer.next());
  EXPECT_EQ(1L, sequencer.remainingCapacity());
}


}
}
