#ifndef __DISRUPTOR_PROCESSINGSEQUENCEBARRIER_HPP__
#define __DISRUPTOR_PROCESSINGSEQUENCEBARRIER_HPP__

#include <atomic>

#include "TimeUnit.hpp"
#include "AlertException.hpp"
#include "SequenceBarrier.hpp"

namespace disruptor {

/**
 * Coordination barrier for tracking the cursor for publishers and sequence of
 * dependent {@link EventProcessor}s for processing a data structure
 */
class ProcessingSequenceBarrier
  : public SequenceBarrier
{
  WaitStrategy& waitStrategy_;
  Sequence& cursorSequence_;
  std::vector<Sequence*> dependentSequences_;
  std::atomic_bool alerted_;
public:
  ProcessingSequenceBarrier(WaitStrategy& waitStrategy,
                            Sequence& cursorSequence,
                            std::vector<Sequence*>& dependentSequences)
    : waitStrategy_(waitStrategy)
    , cursorSequence_(cursorSequence)
    , dependentSequences_(dependentSequences)
    , alerted_(false)
  {}

  long waitFor(long sequence) throw(AlertException) {
    checkAlert();
    return waitStrategy_.waitFor(sequence, cursorSequence_, dependentSequences_, *this);
  }

  long waitFor(long sequence, long timeout, TimeUnit units) throw(AlertException) {
    checkAlert();
    return waitStrategy_.waitFor(sequence, cursorSequence_, dependentSequences_, *this, timeout, units);
  }

  long getCursor() {
    return cursorSequence_.get();
  }

  bool isAlerted() {
    return alerted_;
  }

  void alert() {
    alerted_ = true;
    waitStrategy_.signalAllWhenBlocking();
  }

  void clearAlert() {
    alerted_ = false;
  }

  void checkAlert() throw(AlertException) {
    if (alerted_) {
      throw AlertException("");
    }
  }

public:
  ~ProcessingSequenceBarrier() {}
};

}

#endif /* __DISRUPTOR_PROCESSINGSEQUENCEBARRIER_HPP__ */
