#ifndef __DISRUPTOR_ABSTRACTMULTITHREADEDCLAIMSTRATEGY_HPP__
#define __DISRUPTOR_ABSTRACTMULTITHREADEDCLAIMSTRATEGY_HPP__

#include <thread>
#include <chrono>

#include "ClaimStrategy.hpp"
#include "Sequencer.hpp"
#include "PaddedLong.hpp"
#include "Util.hpp"

namespace disruptor {

class AbstractMultithreadedClaimStrategy {

  const int bufferSize_;
  Sequence claimSequence_;
  long minGatingSequence_;

public:
  AbstractMultithreadedClaimStrategy(const int bufferSize)
    : bufferSize_(bufferSize)
    , claimSequence_(Sequencer::INITIAL_CURSOR_VALUE)
    , minGatingSequence_(Sequencer::INITIAL_CURSOR_VALUE)
  {}

  virtual const int getBufferSize() const {
    return bufferSize_;
  }

  virtual long getSequence() const {
    return claimSequence_.get();
  }

  virtual bool hasAvailableCapacity(const int availableCapacity, std::vector<Sequence*>& dependentSequences) {
    return hasAvailableCapacity(claimSequence_.get(), availableCapacity, dependentSequences);
  }

  virtual long incrementAndGet(std::vector<Sequence*>& dependentSequences) {
    long minGatingSequence = minGatingSequence_;
    waitForCapacity(dependentSequences, minGatingSequence);

    const long nextSequence = claimSequence_.incrementAndGet();
    waitForFreeSlotAt(nextSequence, dependentSequences, minGatingSequence);

    return nextSequence;
  }

  virtual long checkAndIncrement(const int availableCapacity, const int delta, std::vector<Sequence*>& gatingSequences)
    throw(InsufficientCapacityException)
  {
    for (;;) {
      long sequence = claimSequence_.get();
      if (hasAvailableCapacity(sequence, availableCapacity, gatingSequences)) {
        long nextSequence = sequence + delta;
        if (claimSequence_.compareAndSet(sequence, nextSequence)) {
          return nextSequence;
        }
      }
      else {
        throw InsufficientCapacityException("");
      }
    }
  }

  virtual long incrementAndGet(const int delta, std::vector<Sequence*>& dependentSequences) {
    const long nextSequence = claimSequence_.addAndGet(delta);
    waitForFreeSlotAt(nextSequence, dependentSequences, minGatingSequence_);

    return nextSequence;
  }

  virtual void setSequence(const long sequence, std::vector<Sequence*>& dependentSequences) {
    claimSequence_.set(sequence);
    waitForFreeSlotAt(sequence, dependentSequences, minGatingSequence_);
  }

  virtual void serialisePublishing(const long sequence, Sequence& cursor, const long batchSize) { }

private:
  void waitForCapacity(std::vector<Sequence*>& dependentSequences, long& minGatingSequence) {
    const long wrapPoint = (claimSequence_.get() + 1L) - bufferSize_;
    if (wrapPoint > minGatingSequence) {
      long minSequence;
      while (wrapPoint > (minSequence = util::getMinimumSequence(dependentSequences))) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1L));
      }

      /* XXX: should be reference?  or not? */
      minGatingSequence = minSequence;
    }
  }

  void waitForFreeSlotAt(const long sequence, std::vector<Sequence*>& dependentSequences, long& minGatingSequence) {
    const long wrapPoint = sequence - bufferSize_;
    if (wrapPoint > minGatingSequence) {
      long minSequence;
      while (wrapPoint > (minSequence = util::getMinimumSequence(dependentSequences))) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1L));
      }

      /* XXX: should be reference?  or not? */
      minGatingSequence = minSequence;
    }
  }

  bool hasAvailableCapacity(const long sequence, const int availableCapacity, std::vector<Sequence*>& dependentSequences) {
    const long wrapPoint = (sequence + availableCapacity) - bufferSize_;
    long minGatingSequence = minGatingSequence_;

    if (wrapPoint > minGatingSequence) {
      long minSequence = util::getMinimumSequence(dependentSequences);
      minGatingSequence_ = minSequence;

      if (wrapPoint > minSequence) {
        return false;
      }
    }

    return true;
  }

  AbstractMultithreadedClaimStrategy(const AbstractMultithreadedClaimStrategy&) = delete;
  AbstractMultithreadedClaimStrategy& operator=(const AbstractMultithreadedClaimStrategy&) = delete;

protected:
  ~AbstractMultithreadedClaimStrategy() {}
};

}

#endif /* __DISRUPTOR_ABSTRACTMULTITHREADEDCLAIMSTRATEGY_HPP__ */
