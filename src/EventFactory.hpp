#ifndef __DISRUPTOR_EVENTFACTORY_HPP__
#define __DISRUPTOR_EVENTFACTORY_HPP__

namespace disruptor {

/**
 * Called by the {@link RingBuffer} to pre-populate all the events to fill the RingBuffer.
 * 
 * @param <T> event implementation storing the data for sharing during exchange or parallel coordination of an event.
 */
template <typename T>
class EventFactory {
  T newInstance();
protected:
  ~EventFactory() {};
};

}

#endif /* __DISRUPTOR_EVENTFACTORY_HPP__ */
