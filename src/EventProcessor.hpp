#ifndef __DISRUPTOR_EVENTPROCESSOR_HPP__
#define __DISRUPTOR_EVENTPROCESSOR_HPP__

namespace disruptor {
/**
 * EventProcessors waitFor events to become available for consumption from the {@link RingBuffer}
 *
 * An EventProcessor will be associated with a Thread for execution.
 */
class EventProcessor {
 public:
  /**
   * Get a reference to the {@link Sequence} being used by this {@link EventProcessor}.
   *
   * @return reference to the {@link Sequence} for this {@link EventProcessor}
   */
  virtual Sequence& getSequence() = 0;

  /**
   * Signal that this EventProcessor should stop when it has finished consuming at the next clean break.
   * It will call {@link SequenceBarrier#alert()} to notify the thread to check status.
   */
  virtual void halt() = 0;

  /**
   * Implementations of EventProcessor must be a functor (Runnable).
   */
  virtual void operator()() = 0;

protected:
  ~EventProcessor() {}
};

}

#endif /*  __DISRUPTOR_EVENTPROCESSOR_HPP__ */



