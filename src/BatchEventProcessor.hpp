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
#ifndef __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__
#define __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__

#include <atomic>

#include "RingBuffer.hpp"
#include "SequenceBarrier.hpp"
#include "LifecycleAwareEventHandler.hpp"
#include "Sequencer.hpp"
#include "Sequence.hpp"
#include "EventProcessor.hpp"

#include "IllegalStateException.hpp"
#include "FatalExceptionHandler.hpp"

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

  FatalExceptionHandler defaultExceptionHandler_;
  ExceptionHandler* exceptionHandler_;

  RingBuffer<T>& ringBuffer_;
  SequenceBarrier& sequenceBarrier_;
  LifecycleAwareEventHandler<T>& eventHandler_;
  Sequence sequence_;

 public:
  BatchEventProcessor(RingBuffer<T>& ringBuffer, SequenceBarrier& sequenceBarrier, LifecycleAwareEventHandler<T>& eventHandler)
      : running_(false)
      , exceptionHandler_(&defaultExceptionHandler_)
      , ringBuffer_(ringBuffer)
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
   * Set a new ExceptionHandler for handling exceptions propagated out of the BatchEventProcessor.
   */
  void setExceptionHandler(ExceptionHandler& exceptionHandler) {
    exceptionHandler_ = &exceptionHandler;
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
      catch (std::exception ex) {
        exceptionHandler_->handleEventException(ex, nextSequence);
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
    catch (std::exception& ex) {
      exceptionHandler_->handleOnStartException(ex);
    }
  }

  void notifyShutdown() {
    try {
      eventHandler_.onShutdown();
    }
    catch (std::exception& ex) {
      exceptionHandler_->handleOnShutdownException(ex);
    }
  }

};


}

#endif /* __DISRUPTOR_BATCHEVENTPROCESSOR_HPP__ */
