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

  /* bool await(std::chrono::duration ...) { } */

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



