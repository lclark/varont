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
