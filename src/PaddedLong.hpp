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

