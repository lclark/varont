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
#ifndef __VARONT_SEQUENCEBARRIER_HPP__
#define __VARONT_SEQUENCEBARRIER_HPP__

#include "TimeUnit.hpp"
#include "AlertException.hpp"

namespace varont {

/**
 * Coordination barrier for tracking the cursor for publishers and sequence of
 * dependent {@link EventProcessor}s for processing a data structure
 */
class SequenceBarrier {
public:
  /**
   * Wait for the given sequence to be available for consumption.
   *
   * @param sequence to wait for
   * @return the sequence up to which is available
   * @throws AlertException if a status change has occurred for the Disruptor
   * @throws InterruptedException if the thread needs awaking on a condition variable.
   */
  virtual long waitFor(long sequence) throw(AlertException) = 0;

  /**
   * Wait for the given sequence to be available for consumption with a time out.
   *
   * @param sequence to wait for
   * @param timeout value
   * @param units for the timeout value
   * @return the sequence up to which is available
   * @throws AlertException if a status change has occurred for the Disruptor
   * @throws InterruptedException if the thread needs awaking on a condition variable.
   */
  virtual long waitFor(long sequence, long timeout, TimeUnit units) throw(AlertException) = 0;

  /**
   * Delegate a call to the {@link Sequencer#getCursor()}
   *
   * @return value of the cursor for entries that have been published.
   */
  virtual long getCursor() = 0;

  /**
   * The current alert status for the barrier.
   *
   * @return true if in alert otherwise false.
   */
  virtual bool isAlerted() = 0;

  /**
   * Alert the {@link EventProcessor}s of a status change and stay in this status until cleared.
   */
  virtual void alert() = 0;

  /**
   * Clear the current alert status.
   */
  virtual void clearAlert() = 0;

  /**
   * Check if an alert has been raised and throw an {@link AlertException} if it has.
   *
   * @throws AlertException if alert has been raised.
   */
  virtual void checkAlert() throw(AlertException) = 0;

protected:
  ~SequenceBarrier() {}
};

}

#endif /* __VARONT_SEQUENCEBARRIER_HPP__ */



