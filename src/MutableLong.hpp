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



