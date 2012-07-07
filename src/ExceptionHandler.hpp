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

