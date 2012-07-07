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
#ifndef __DISRUPTOR_ALERTEXCEPTION_HPP__
#define __DISRUPTOR_ALERTEXCEPTION_HPP__

#include <stdexcept>

namespace disruptor {

class AlertException
  : public std::runtime_error
{
public:
  explicit AlertException(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit AlertException(const char* what_arg) : runtime_error(what_arg) {}
};

}

#endif /* __DISRUPTOR_ALERTEXCEPTION_HPP__ */
