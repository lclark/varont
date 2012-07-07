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
#ifndef __DISRUPTOR_MULTITHREADEDLOWCONTENTIONCLAIMSTRATEGY_HPP__
#define __DISRUPTOR_MULTITHREADEDLOWCONTENTIONCLAIMSTRATEGY_HPP__

#include "AbstractMultithreadedClaimStrategy.hpp"

namespace disruptor {

class MultiThreadedLowContentionClaimStrategy
    : public AbstractMultithreadedClaimStrategy
{
 public:
  /**
   * Construct a new multi-threaded publisher {@link ClaimStrategy} for a given buffer size.
   *
   * @param bufferSize for the underlying data structure.
   */
  MultiThreadedLowContentionClaimStrategy(const int bufferSize)
      : AbstractMultithreadedClaimStrategy(bufferSize)
  {}

  void serialisePublishing(const long sequence, Sequence& cursor, const int batchSize) {
    long expectedSequence = sequence - batchSize;
    while (expectedSequence != cursor.get()) {
      // busy spin
    }

    cursor.set(sequence);
  }  
};

}

#endif /* __DISRUPTOR_MULTITHREADEDLOWCONTENTIONCLAIMSTRATEGY_HPP__ */


