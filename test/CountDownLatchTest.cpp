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

TEST_F(CountDownLatchTest, NoWaitInitialCountZeroWithinTimeout) {
  CountDownLatch latch(0);
  EXPECT_EQ(0, latch.getCount());

  bool done = false;

  std::thread thread0([&] {
      /* await should return immediately, timeout ineffective. */
      ASSERT_TRUE(latch.await(std::chrono::seconds(10)));
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

TEST_F(CountDownLatchTest, SingleThreadLatchCoordinationWithinTimeout) {
  CountDownLatch latch(1);
  EXPECT_EQ(1, latch.getCount());

  std::atomic_bool done(false);

  std::thread thread0([&] {
      /* the latch should be released long before this times out */
      ASSERT_TRUE(latch.await(std::chrono::seconds(10)));
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

TEST_F(CountDownLatchTest, SingleThreadLatchCoordinationWithTimeout) {
  CountDownLatch latch(1);
  EXPECT_EQ(1, latch.getCount());

  std::atomic_bool done(false);

  std::thread thread0([&] {
      /* the latch will not be released */
      ASSERT_FALSE(latch.await(std::chrono::seconds(1)));
      done = true;
    });

  /* Give thread0 enough time to have started before we proceed. */
  std::this_thread::sleep_for(std::chrono::milliseconds(5));

  EXPECT_FALSE(done);

  /* Give thread0 enough time to timeout */
  std::this_thread::sleep_for(std::chrono::seconds(2));

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
