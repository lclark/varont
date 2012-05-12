#ifndef __DISRUPTOR_INSUFFICIENTCAPACITYEXCEPTION_HPP__
#define __DISRUPTOR_INSUFFICIENTCAPACITYEXCEPTION_HPP__

#include <stdexcept>

namespace disruptor {

class InsufficientCapacityException
  : public std::runtime_error
{
public:
  explicit InsufficientCapacityException(const std::string& what_arg) : runtime_error(what_arg) {}
  explicit InsufficientCapacityException(const char* what_arg) : runtime_error(what_arg) {}
};

}

#endif /* __DISRUPTOR_INSUFFICIENTCAPACITYEXCEPTION_HPP__ */
