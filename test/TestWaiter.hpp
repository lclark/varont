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
