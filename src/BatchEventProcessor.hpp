#ifndef __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__
#define __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__

#include <atomic>

#include "RingBuffer.hpp"
#include "SequenceBarrier.hpp"
#include "LifecycleAwareEventHandler.hpp"
#include "Sequencer.hpp"
#include "Sequence.hpp"

#include "IllegalStateException.hpp"

namespace disruptor {

/**
 * Convenience class for handling the batching semantics of consuming entries from a {@link RingBuffer}
 * and delegating the available events to a {@link EventHandler}.
 *
 * If the {@link EventHandler} also implements {@link LifecycleAware} it will be notified just after the thread
 * is started and just before the thread is shutdown.
 *
 * @param <T> event implementation storing the data for sharing during exchange or parallel coordination of an event.
 */
template <typename T>
class BatchEventProcessor
    : public EventProcessor
{
  std::atomic_bool running_;
    // private ExceptionHandler exceptionHandler = new FatalExceptionHandler();

  RingBuffer<T>& ringBuffer_;
  SequenceBarrier& sequenceBarrier_;
  LifecycleAwareEventHandler<T>& eventHandler_;
  Sequence sequence_;

 public:
  BatchEventProcessor(RingBuffer<T>& ringBuffer, SequenceBarrier& sequenceBarrier, LifecycleAwareEventHandler<T>& eventHandler)
      : ringBuffer_(ringBuffer)
      , sequenceBarrier_(sequenceBarrier)
      , eventHandler_(eventHandler)
      , sequence_(Sequencer::INITIAL_CURSOR_VALUE)
  {}

  Sequence& getSequence() {
    return sequence_;
  }

  void halt() {
    running_.store(false);
    sequenceBarrier_.alert();
  }

  /**
   * It is ok to have another thread rerun this method after a halt().
   */
  void operator()() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
      throw new IllegalStateException("Thread is already running");
    }

    sequenceBarrier_.clearAlert();

    notifyStart();

    T* event = nullptr;
    long nextSequence = sequence_.get() + 1L;

    while (true) {
      try {
        long availableSequence = sequenceBarrier_.waitFor(nextSequence);
        while (nextSequence <= availableSequence) {
          event = &ringBuffer_.get(nextSequence);
          eventHandler_.onEvent(*event, nextSequence, nextSequence == availableSequence);
          nextSequence++;
        }

        sequence_.set(nextSequence - 1L);
      }
      catch (AlertException ex) {
        if (!running_.load()) {
          break;
        }
      }
      catch (...) {
        // exceptionHandler.handleEventException(ex, nextSequence, event);
        sequence_.set(nextSequence);
        nextSequence++;
      }
    }

    notifyShutdown();

    running_.store(false);
  }

 private:
  void notifyStart() {
    try {
      eventHandler_.onStart();
    }
    catch (...) {
      // exceptionHandler_.handleOnStartException
    }
  }

  void notifyShutdown() {
    try {
      eventHandler_.onShutdown();
    }
    catch (...) {
      // exceptionHandler_.handleOnShutdownException
    }
  }

};


}

#endif /* __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__ */
