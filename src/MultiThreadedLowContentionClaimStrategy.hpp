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


