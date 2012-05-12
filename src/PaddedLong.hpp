#ifndef __DISRUPTOR_PADDEDLONG_HPP__
#define __DISRUPTOR_PADDEDLONG_HPP__

#include "MutableLong.hpp"

namespace disruptor {
namespace util {

/**
 * Cache line padded long variable to be used when false sharing may
 * be an issue.
 */
class PaddedLong
  : public MutableLong
{
public:
  long p1, p2, p3, p4, p5, p6;

  /**
   * Construct with initial value of 0L
   */
  PaddedLong()
    : MutableLong()
    , p1(7), p2(7), p3(7), p4(7), p5(7), p6(7)
  {}

  /**
   * Construct with specified initial value
   */
  PaddedLong(const long initialValue)
    : MutableLong(initialValue)
    , p1(7), p2(7), p3(7), p4(7), p5(7), p6(7)
  {}

  long sumPaddingToPreventOptimisation() {
    return p1 + p2 + p3 + p4 + p5 + p6;
  }
  
};

}
}

#endif /* __DISRUPTOR_PADDEDLONG_HPP__ */

