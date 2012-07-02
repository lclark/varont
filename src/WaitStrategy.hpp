#ifndef __DISRUPTOR_WAITSTRATEGY_HPP__
#define __DISRUPTOR_WAITSTRATEGY_HPP__

#include <vector>
#include <chrono>

#include "TimeUnit.hpp"
#include "AlertException.hpp"

namespace disruptor {
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

#endif /* __DISRUPTOR_WAITSTRATEGY_HPP__ */
