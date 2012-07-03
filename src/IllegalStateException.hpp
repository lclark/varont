#ifndef __DISRUPTOR_ILLEGALSTATEEXCEPTION_HPP__
#define __DISRUPTOR_ILLEGALSTATEEXCEPTION_HPP__

#include <stdexcept>

namespace disruptor {

class IllegalStateException
  : public std::runtime_error
{
public:
  explicit IllegalStateException(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit IllegalStateException(const char* what_arg) : runtime_error(what_arg) {}
};

}

#endif /* __DISRUPTOR_ILLEGALSTATEEXCEPTION_HPP__ */
