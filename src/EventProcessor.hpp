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
#ifndef __VARONT_EVENTPROCESSOR_HPP__
#define __VARONT_EVENTPROCESSOR_HPP__

namespace varont {
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

#endif /*  __VARONT_EVENTPROCESSOR_HPP__ */



