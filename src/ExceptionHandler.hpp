#ifndef __DISRUPTOR_EXCEPTIONHANDLER_HPP__
#define __DISRUPTOR_EXCEPTIONHANDLER_HPP__

#include <exception>

namespace disruptor {

/**
 * Callback handler for uncaught exceptions in the event processing cycle of the {@link BatchEventProcessor}
 */
class ExceptionHandler {
public:
  /**
   * Strategy for handling uncaught exceptions when processing an event.
   *
   * If the strategy wishes to suspend further processing by the {@link BatchEventProcessor}
   * then is should throw a {@link RuntimeException}.
   *
   * @param ex the exception that propagated from the {@link EventHandler}.
   * @param sequence of the event which cause the exception.
   * @param event being processed when the exception occurred.
   */
  virtual void handleEventException(std::exception& ex, long sequence) = 0;

  /**
   * Callback to notify of an exception during {@link LifecycleAware#onStart()}
   *
   * @param ex throw during the starting process.
   */
  virtual void handleOnStartException(std::exception& ex) = 0;

  /**
   * Callback to notify of an exception during {@link LifecycleAware#onShutdown()}
   *
   * @param ex throw during the shutdown process.
   */
  virtual void handleOnShutdownException(std::exception& ex) = 0;
};

}

#endif /* __DISRUPTOR_EXCEPTIONHANDLER_HPP__ */

