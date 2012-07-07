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
#ifndef __VARONT_BLOCKINGWAITSTRATEGY_HPP__
#define __VARONT_BLOCKINGWAITSTRATEGY_HPP__

#include <vector>
#include <chrono>
#include <mutex>
#include <condition_variable>

#include "SequenceBarrier.hpp"
#include "WaitStrategy.hpp"
#include "Util.hpp"
#include "TimeUnit.hpp"

namespace varont {

class WaiterGuard {
  int& numWaiters_;
public:
  WaiterGuard(int &numWaiters)
    : numWaiters_(numWaiters)
  {
    ++numWaiters_;
  }

  ~WaiterGuard() {
    --numWaiters_;
  }
};

/**
 * Blocking strategy that uses a lock and condition variable for {@link EventProcessor}s waiting on a barrier.
 *
 * This strategy can be used when throughput and low-latency are not as important as CPU resource.
 */
class BlockingWaitStrategy
  : public WaitStrategy
{
  std::mutex lock_;
  std::condition_variable processorNotifyCondition_;
  int numWaiters_;

public:
  long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*>& dependents, SequenceBarrier& barrier)
    throw(AlertException)
  {
    long availableSequence;

    if ((availableSequence = cursor.get()) < sequence) {
      std::unique_lock<std::mutex> lock(lock_);
      WaiterGuard waiterGuard(numWaiters_);

      while ((availableSequence = cursor.get()) < sequence) {
        barrier.checkAlert();
        processorNotifyCondition_.wait(lock);
      }
    }

    if (0 != dependents.size()) {
      while ((availableSequence = util::getMinimumSequence(dependents)) < sequence) {
        barrier.checkAlert();
      }
    }

    return availableSequence;
  }

  long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*>& dependents, SequenceBarrier& barrier,
               long timeout, TimeUnit sourceUnit)
    throw(AlertException)
  {
    long availableSequence;

    if ((availableSequence = cursor.get()) < sequence) {
      std::unique_lock<std::mutex> lock(lock_);
      WaiterGuard waiterGuard(numWaiters_);

      while ((availableSequence = cursor.get()) < sequence) {
        barrier.checkAlert();

        if (std::cv_status::timeout == processorNotifyCondition_.wait_for(lock, std::chrono::milliseconds(timeout))) {
          break;
        }
      }
    }

    if (0 != dependents.size()) {
      while ((availableSequence = util::getMinimumSequence(dependents)) < sequence) {
        barrier.checkAlert();
      }
    }

    return availableSequence;
  }

  void signalAllWhenBlocking() {
    if (0 != numWaiters_) {
      std::lock_guard<std::mutex> lock(lock_);
      processorNotifyCondition_.notify_all();
    }
  }


};

}

#endif /* __VARONT_BLOCKINGWAITSTRATEGY_HPP__ */
