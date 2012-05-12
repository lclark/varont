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
