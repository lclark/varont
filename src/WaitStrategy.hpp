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
#ifndef __VARONT_WAITSTRATEGY_HPP__
#define __VARONT_WAITSTRATEGY_HPP__

#include <vector>
#include <chrono>

#include "TimeUnit.hpp"
#include "AlertException.hpp"

namespace varont {
class SequenceBarrier;

/**
 * Strategy employed for making {@link EventProcessor}s wait on a cursor {@link Sequence}.
 */
class WaitStrategy {
public:
  /**
   * Wait for the given sequence to be available
   *
   * @param sequence to be waited on.
   * @param cursor on which to wait.
   * @param dependents further back the chain that must advance first
   * @param barrier the processor is waiting on.
   * @return the sequence that is available which may be greater than the requested sequence.
   * @throws AlertException if the status of the Disruptor has changed.
   * @throws InterruptedException if the thread is interrupted.
   */
  virtual long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*>& dependents, SequenceBarrier& barrier)
    throw(AlertException /*, InterruptedException */) = 0;

  /**
   * Wait for the given sequence to be available with a timeout specified.
   *
   * @param sequence to be waited on.
   * @param cursor on which to wait.
   * @param dependents further back the chain that must advance first
   * @param barrier the processor is waiting on.
   * @param timeout value to abort after.
   * @param sourceUnit of the timeout value.
   * @return the sequence that is available which may be greater than the requested sequence.
   * @throws AlertException if the status of the Disruptor has changed.
   * @throws InterruptedException if the thread is interrupted.
   */
  virtual long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*>& dependents, SequenceBarrier& barrier,
                 long timeout, TimeUnit sourceUnit)
    throw(AlertException /*, InterruptedException */) = 0;

  /**
   * Signal those {@link EventProcessor}s waiting that the cursor has advanced.
   */
  virtual void signalAllWhenBlocking() = 0;

protected:
  ~WaitStrategy() {}
};

}

#endif /* __VARONT_WAITSTRATEGY_HPP__ */
