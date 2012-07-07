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
#ifndef __VARONT_PROCESSINGSEQUENCEBARRIER_HPP__
#define __VARONT_PROCESSINGSEQUENCEBARRIER_HPP__

#include <atomic>

#include "TimeUnit.hpp"
#include "AlertException.hpp"
#include "SequenceBarrier.hpp"

namespace varont {

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

#endif /* __VARONT_PROCESSINGSEQUENCEBARRIER_HPP__ */
