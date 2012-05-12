#ifndef __DISRUPTOR_SLEEPINGWAITSTRATEGY_HPP__
#define __DISRUPTOR_SLEEPINGWAITSTRATEGY_HPP__

#include <vector>
#include <chrono>

#include "SequenceBarrier.hpp"
#include "WaitStrategy.hpp"
#include "Util.hpp"
#include "TimeUnit.hpp"

namespace disruptor {
/**
 * Sleeping strategy that initially spins, then uses a Thread.yield(), and eventually for the minimum number of nanos
 * the OS and JVM will allow while the {@link com.lmax.disruptor.EventProcessor}s are waiting on a barrier.
 *
 * This strategy is a good compromise between performance and CPU resource. Latency spikes can occur after quiet periods.
 */
class SleepingWaitStrategy
  : public WaitStrategy
{
  static const int RETRIES = 200;

public:
  long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*> dependents, SequenceBarrier& barrier)
    throw(AlertException/*, InterruptedException*/)
  {
    long availableSequence;
    int counter = RETRIES;

    if (dependents.empty()) {
      while ((availableSequence = cursor.get()) < sequence) {
        counter = applyWaitMethod(barrier, counter);
      }
    }
    else {
      while ((availableSequence = util::getMinimumSequence(dependents)) < sequence)
        {
          counter = applyWaitMethod(barrier, counter);
        }
    }

    return availableSequence;
  }

  long waitFor(long sequence, Sequence& cursor, std::vector<Sequence*> dependents, SequenceBarrier& barrier,
               long timeout, TimeUnit sourceUnit)
    throw(AlertException/*, InterruptedException*/)
  {
    long timeoutMs = timeout; /*sourceUnit.toMillis(timeout);*/
    auto startTime = std::chrono::system_clock::now();
    long availableSequence;
    int counter = RETRIES;

    if (dependents.empty()) {
      while ((availableSequence = cursor.get()) < sequence) {
        counter = applyWaitMethod(barrier, counter);

        auto elapsedTime = std::chrono::system_clock::now() - startTime;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() > timeoutMs) {
          break;
        }
      }
    }
    else {
      while ((availableSequence = util::getMinimumSequence(dependents)) < sequence) {
        counter = applyWaitMethod(barrier, counter);

        auto elapsedTime = std::chrono::system_clock::now() - startTime;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count() > timeoutMs) {
          break;
        }
      }
    }
    
    return availableSequence;
  }

  void signalAllWhenBlocking() {
  }

  int applyWaitMethod(SequenceBarrier& barrier, int counter)
    throw(AlertException)
  {
    barrier.checkAlert();

    if (counter > 100) {
      --counter;
    }
    else if (counter > 0) {
      --counter;
      std::this_thread::yield();
    }
    else {
      std::this_thread::sleep_for(std::chrono::nanoseconds(1L));
    }

    return counter;
  }
};

}

#endif /* __DISRUPTOR_SLEEPINGWAITSTRATEGY_HPP__ */
