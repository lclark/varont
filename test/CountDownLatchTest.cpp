#include "CountDownLatch.hpp"

#include <atomic>
#include <thread>

#include <gtest/gtest.h>

struct CountDownLatchTest : public testing::Test {
  CountDownLatchTest() { }
};

TEST_F(CountDownLatchTest, CheckInitialCount) {
  CountDownLatch latch(5);
  EXPECT_EQ(5, latch.getCount());
}

TEST_F(CountDownLatchTest, CountDown) {
  CountDownLatch latch(5);
  latch.countDown();
  EXPECT_EQ(4, latch.getCount());
}

TEST_F(CountDownLatchTest, NoWaitInitialCountZero) {
  CountDownLatch latch(0);
  EXPECT_EQ(0, latch.getCount());

  bool done = false;

  std::thread thread0([&] {
      latch.await();
      done = true;
    });

  /* Give thread0 enough time to have started before we proceed. */
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(done);

  thread0.join();
}

TEST_F(CountDownLatchTest, SingleThreadLatchCoordination) {
  CountDownLatch latch(1);
  EXPECT_EQ(1, latch.getCount());

  std::atomic_bool done(false);

  std::thread thread0([&] {
      latch.await();
      done = true;
    });

  /* Give thread0 enough time to have started before we proceed. */
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_FALSE(done);

  latch.countDown();

  /* Give thread0 enough time to respond to the condition */
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(done);

  thread0.join();
}

TEST_F(CountDownLatchTest, MultiThreadedLatchCoordination) {
  CountDownLatch latch(2);
  std::atomic_bool done(false);

  std::thread thread0([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      latch.countDown();
    });

  std::thread thread1([&] {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      latch.countDown();
    });

  std::thread thread2([&] {
      latch.await();
      done = true;
    });

  thread0.join();
  thread1.join();

  /* Give latch.await() in thread2 enough time to wake up. */
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_TRUE(done);


  thread2.join();
}
