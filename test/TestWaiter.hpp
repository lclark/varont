#ifndef __DISRUPTOR_TESTWAITER_HPP__
#define __DISRUPTOR_TESTWAITER_HPP__

namespace disruptor {
namespace test {

class TestWaiter {
  const long initialSequence_;
  const long toWaitForSequence_;
  CyclicBarrier& cyclicBarrier_;
  SequenceBarrier& sequenceBarrier_;
  RingBuffer<StubEvent>& ringBuffer_;

public:
  TestWaiter(CyclicBarrier&,
             SequenceBarrier&,
             RingBuffer<StubEvent>&,
             const long initialSequence,
             const long toWaitForSequence)
      : initialSequence_(initialSequence)
      , toWaitForSequence_(toWaitForSequence)
      , cyclicBarrier_(cyclicBarrier)
      , sequenceBarrier_(sequenceBarrier)
      , ringBuffer_(ringBuffer)
  {
  }

  int operator()() {
    cyclicBarrier.await();
    sequenceBarrier.waitFor(toWaitForSequence);

    
  }
}

}
}

#endif /* __DISRUPTOR_TESTWAITER_HPP__ */
