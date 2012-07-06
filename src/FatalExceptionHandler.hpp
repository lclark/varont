#ifndef __DISRUPTOR_FATALEXCEPTIONHANDLER_HPP__
#define __DISRUPTOR_FATALEXCEPTIONHANDLER_HPP__

#include <exception>

#include "ExceptionHandler.hpp"

namespace disruptor {

class FatalExceptionHandler
    : public ExceptionHandler
{
public:
  void handleEventException(std::exception& ex, long sequence) {
    throw std::runtime_error("BatchEventProcessor FATAL unhandled exception while processing event.");
  }

  void handleOnStartException(std::exception& ex) { }

  void handleOnShutdownException(std::exception& ex) { }
};

}

#endif /* __DISRUPTOR_FATALEXCEPTIONHANDLER_HPP__ */

