#include "Util.hpp"
#include "Sequence.hpp"

#include <limits>

namespace disruptor {
namespace util {

long getMinimumSequence(std::vector<Sequence*> sequences) {
  long minimum = std::numeric_limits<long>::max();

  for (Sequence* sequence : sequences) {
    long value = sequence->get();
    minimum = std::min(minimum, value);
  }

  return minimum;
}

int bitCount(int v) {
  int count = 0;
  int mask = 0;
  int i = 0;
  for (mask = 1u, i = 0; mask; mask <<= 1, ++i) {
    count += (v & mask) >> i;
  }
  return count;
}

}
}
