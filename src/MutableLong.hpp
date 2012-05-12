#ifndef __DISRUPTOR_MUTABLELONG_HPP__
#define __DISRUPTOR_MUTABLELONG_HPP__

namespace disruptor {
namespace util {

class MutableLong {
  long value_;
public:
  /**
   * Construct with initial value of 0.
   */
  MutableLong()
    : value_(0L)
  {}

  /**
   * Construct with specified initial value
   *
   * @param initialValue Initial value.
   */
  MutableLong(const long initialValue)
    : value_(initialValue)
  {}

  /**
   * Retrieve the current value
   *
   * @return the value.
   */
  const long get() const {
    return value_;
  }

  /**
   * Set the value
   *
   * @param value to set.
   */
  void set(const long value) {
    value_ = value;
  }
};

}
}

#endif /* __DISRUPTOR_MUTABLELONG_HPP__ */



