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
#ifndef __DISRUPTOR_AGGREGATEEVENTHANDLER_HPP__
#define __DISRUPTOR_AGGREGATEEVENTHANDLER_HPP__

#include <atomic>

#include "LifecycleAware.hpp"
#include "EventHandler.hpp"

#include "IllegalStateException.hpp"
#include "FatalExceptionHandler.hpp"

namespace disruptor {

template <typename T>
class AggregateEventHandler
    : public EventHandler<T>
    , public LifecycleAware
{
  std::vector<EventHandler<T>*> eventHandlers_;
 public:
  /**
   * Construct an aggregate collection of {@link EventHandler}s to be called in sequence.
   *
   * @param eventHandlers to be called in sequence.
   */
  AggregateEventHandler(std::vector<EventHandler<T>*>& eventHandlers)
      : eventHandlers_(eventHandlers)
  { }

  void onEvent(T& event, long sequence, bool endOfBatch) {
    for (EventHandler<T>* eventHandler : eventHandlers_) {
      eventHandler->onEvent(event, sequence, endOfBatch);
    }
  }

  void onStart() {
    for (EventHandler<T>* eventHandler : eventHandlers_) {
    }
  }

};

}

#endif /* __DISRUPTOR_AGGREGATEEVENTHANDLER_HPP__ */
