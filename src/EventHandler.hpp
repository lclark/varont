#ifndef __DISRUPTOR_EVENTHANDLER_HPP__
#define __DISRUPTOR_EVENTHANDLER_HPP__

namespace disruptor {

/**
 * Callback interface to be implemented for processing events as they become available in the {@link RingBuffer}
 *
 * @see BatchEventProcessor#setExceptionHandler(ExceptionHandler) if you want to handle exceptions propigated out of the handler.
 *
 * @param <T> event implementation storing the data for sharing during exchange or parallel coordination of an event.
 */
template <typename T>
class EventHandler {
 public:
  /**
   * Called when a publisher has published an event to the {@link RingBuffer}
   *
   * @param event published to the {@link RingBuffer}
   * @param sequence of the event being processed
   * @param endOfBatch flag to indicate if this is the last event in a batch from the {@link RingBuffer}
   * @throws Exception if the EventHandler would like the exception handled further up the chain.
   */
  virtual void onEvent(T& event, long sequence, bool endOfBatch) = 0;

 protected:
  ~EventHandler() {}
};

}

#endif /* __DISRUPTOR_EVENTHANDLER_HPP__ */
