#ifndef __CYCLICBARRIER_HPP__
#define __CYCLICBARRIER_HPP__

#include <mutex>
#include <condition_variable>

class CyclicBarrier {
public:
  /**
   * Creates a new CyclicBarrier that will trip when the given number
   * of parties (threads) are waiting upon it.
   *
   * @param parties the number of threads that must invoke await() before barrier is tripped.
   */
  CyclicBarrier(unsigned parties)
    : parties_(parties)
    , waitingParties_(0)
  {}

  /**
   * Waits until all parties have inboked await on this barrier.
   */
  int await() {
    std::unique_lock<std::mutex> lock(m_);
    if (++waitingParties_ < parties_) {
      /* As the latest to arrive, we are not the last one.  Wait. */
      cond_.wait(lock);
    }
    else {
      /* We're the last to arrive.  Wake everyone else up! */
      cond_.notify_all();
    }

    return 0;
  }

private:
  /**
   * Number of expected parties
   */
  const unsigned parties_;

  /**
   * Number of parties laying in wait...
   */
  unsigned waitingParties_;

  std::mutex m_;
  std::condition_variable cond_;
};

#endif /* __CYCLICBARRIER_HPP__ */
