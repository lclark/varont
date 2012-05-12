#ifndef __DISRUPTOR_EVENTPROCESSOR_HPP__
#define __DISRUPTOR_EVENTPROCESSOR_HPP__

namespace disruptor {
/**
 * EventProcessors waitFor events to become available for consumption from the {@link RingBuffer}
 *
 * An EventProcessor will be associated with a Thread for execution.
 */
class EventProcessor {
  /**
   * Get a reference to the {@link Sequence} being used by this {@link EventProcessor}.
   *
   * @return reference to the {@link Sequence} for this {@link EventProcessor}
   */
  Sequence& getSequence();

  /**
   * Signal that this EventProcessor should stop when it has finished consuming at the next clean break.
   * It will call {@link SequenceBarrier#alert()} to notify the thread to check status.
   */
  void halt();

  /**
   * Implementations of EventProcessor must be a functor (Runnable).
   */
  void operator()();

protected:
  ~EventProcessor() {}
};

}

#endif /*  __DISRUPTOR_EVENTPROCESSOR_HPP__ */



