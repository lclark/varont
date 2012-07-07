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
long getMinimumSequence(std::vector<Sequence*>& sequences);

int bitCount(int);

}
}

#endif /* __DISRUPTOR_UTIL_HPP__ */
