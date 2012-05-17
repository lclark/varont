#ifndef __DISRUPTOR_SEQUENCE_HPP__
#define __DISRUPTOR_SEQUENCE_HPP__

#include <atomic>

namespace disruptor {

class Sequence {
  std::atomic_long value_;
public:
  Sequence() {
    setOrdered(-1);
  }

  Sequence(const long initialValue) {
    setOrdered(initialValue);
  }

  Sequence(const Sequence&) = delete;
  Sequence& operator=(const Sequence&) = delete;
  
  const long get() const {
    return value_.load();
  }

  virtual void set(const long value) {
    value_.store(value);
  }

  void setOrdered(const long value) {
    set(value);
  }

  /* Made virtual for multithreadedclaimstrategytest's Sequence_
   * class.  Review and come up with an alternative test. */
  virtual bool compareAndSet(long expectedValue, long newValue) {
    return value_.compare_exchange_strong(expectedValue, newValue);
  }

  long incrementAndGet() {
    /* Atomically adds 1L to the value and returns the previous value,
       which we will then add 1L to and return. */
    return value_.fetch_add(1L) + 1L;
  }

  long addAndGet(const long increment) {
    /* Atomically adds increment to the value and returns the previous
       value, which we will then add increment to and return. */
    return value_.fetch_add(increment) + increment;
  }
};

}

#endif /* __DISRUPTOR_SEQUENCE_HPP__ */
