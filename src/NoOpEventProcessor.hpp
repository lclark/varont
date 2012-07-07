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
#ifndef __DISRUPTOR_NOOPEVENTPROCESSOR_HPP__
#define __DISRUPTOR_NOOPEVENTPROCESSOR_HPP__

#include "EventProcessor.hpp"

namespace disruptor {

class SequencerFollowingSequence
  : public Sequence
{
  Sequencer& sequencer_;
public:
  SequencerFollowingSequence(Sequencer& sequencer) 
    : Sequence(Sequencer::INITIAL_CURSOR_VALUE)
    , sequencer_(sequencer)
  {}

  long get() {
    return sequencer_.getCursor();
  }
};

class NoOpEventProcessor
  : public EventProcessor
{
  SequencerFollowingSequence sequence_;

public:
  /**
   * Construct a {@link EventProcessor} that simply tracks a {@link Sequencer}.
   *
   * @param sequencer to track.
   */
  NoOpEventProcessor(Sequencer& sequencer)
    : sequence_(sequencer)
  {}

  Sequence& getSequence() { return sequence_; }

  void halt() {}

  void operator()() {}
};
  
}

#endif /*  __DISRUPTOR_NOOPEVENTPROCESSOR_HPP__ */
