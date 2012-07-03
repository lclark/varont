#ifndef __DISRUPTOR_LIFECYCLEAWAREEVENTHANDLER_HPP__
#define __DISRUPTOR_LIFECYCLEAWAREEVENTHANDLER_HPP__

#include "LifecycleAware.hpp"
#include "EventHandler.hpp"

namespace disruptor {

template <typename T>
class LifecycleAwareEventHandler
    : public LifecycleAware
    , public EventHandler<T>
{
 protected:
  ~LifecycleAwareEventHandler() {}
};

}

#endif /* __DISRUPTOR_LIFECYCLEAWAREEVENTHANDLER_HPP__ */
