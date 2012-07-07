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
#include "Util.hpp"
#include "Sequence.hpp"

#include <limits>

namespace varont {
namespace util {

long getMinimumSequence(std::vector<Sequence*>& sequences) {
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
