#ifndef __DISRUPTOR_UTIL_HPP__
#define __DISRUPTOR_UTIL_HPP__

#include <vector>

namespace disruptor {

class Sequence;

namespace util {

/**
 * Get the minimum sequence from an array of {@link com.lmax.disruptor.Sequence}s.
 *
 * @param sequences to compare.
 * @return the minimum sequence found or Long.MAX_VALUE if the array is empty.
 */
long getMinimumSequence(std::vector<Sequence*> sequences);

int bitCount(int);

}
}

#endif /* __DISRUPTOR_UTIL_HPP__ */
