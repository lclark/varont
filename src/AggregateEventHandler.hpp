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
