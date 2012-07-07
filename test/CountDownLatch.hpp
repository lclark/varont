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
#ifndef __COUNTDOWNLATCH_HPP__
#define __COUNTDOWNLATCH_HPP__

#include <mutex>
#include <condition_variable>

class CountDownLatch {
public:
  /**
   *
   * @param initialCount   the number of times countDown() must be invoked
   *                       before threads can pass through await() (see: count_)
   */
  CountDownLatch(unsigned long initialCount)
    : count_(initialCount)
  { }

  ~CountDownLatch() { }

  CountDownLatch(const CountDownLatch&) = delete;
  CountDownLatch& operator=(const CountDownLatch&) = delete;

  /**
   * Causes the current thread to wait until the latch has counted
   * down to zero.
   */
  void await() {
    std::unique_lock<std::mutex> lock(m_);
    if (0 < count_) {
      cond_.wait(lock);
    }
  }

  /**
   * Causes the current thread to wait until the latch has counted
   * down to zero, or the specified waiting time elapses.
   *
   * @return true if the count reached zero and false if the waiting time elapsed before the count reached zero.
   */
  bool await(std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_);
    if (0 < count_) {
      return std::cv_status::no_timeout == cond_.wait_for(lock, duration);
    }
    else {
      /* count reached zero */
      return true;
    }
  }

  /**
   * Decrement the count of the latch, releasing all waiting threads
   * if the count reaches zero.
   */
  void countDown() {
    std::lock_guard<std::mutex> lock(m_);
    if (0 < count_) {
      if (0 == --count_) {
        /* release latch */
        cond_.notify_all();
      }
    }
  }

  /**
   * Returns the current count.
   */
  const unsigned long getCount() const {
    return count_;
  }
private:
  /**
   * The number of times countDown() must be invoked before threads
   * can pass through await.
   */
  unsigned long count_;

  std::mutex m_;
  std::condition_variable cond_;
};

#endif /* __COUNTDOWNLATCH_HPP__ */



