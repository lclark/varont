#ifndef __DISRUPTOR_RINGBUFFER_HPP__
#define __DISRUPTOR_RINGBUFFER_HPP__

#include <stdexcept>

#include "Sequencer.hpp"
#include "Util.hpp"

namespace disruptor {

template <typename T>
class RingBuffer
  : public Sequencer
{
  int indexMask_;
  T* entries_;

public:

  /**
   * Construct a RingBuffer with the full option set.
   *
   * @param claimStrategy threading strategy for publisher claiming entries in the ring.
   * @param waitStrategy waiting strategy employed by processorsToTrack waiting on entries becoming available.
   *
   * @throws IllegalArgumentException if bufferSize is not a power of 2
   */
  RingBuffer(ClaimStrategy& claimStrategy, WaitStrategy& waitStrategy)

    : Sequencer(claimStrategy, waitStrategy)
    , indexMask_(claimStrategy.getBufferSize() - 1)
    , entries_(nullptr)
  {
    if (util::bitCount(claimStrategy.getBufferSize()) != 1) {
      throw std::out_of_range("bufferSize must be a power of 2");
    }

    entries_ = new T[claimStrategy.getBufferSize()];
  }

  ~RingBuffer() {
    if (nullptr != entries_) {
      delete [] entries_;
    }
  }

  /**
   * Get the event for a given sequence in the RingBuffer.
   *
   * @param sequence for the event
   * @return event for the sequence
   */
  T& get(const long sequence) {
    return entries_[(int)sequence & indexMask_];
  }

  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
  RingBuffer(RingBuffer&&) = delete;
  RingBuffer& operator=(RingBuffer&&) = delete;
};

  // /**
  //  * Construct a RingBuffer with default strategies of:
  //  * {@link MultiThreadedClaimStrategy} and {@link BlockingWaitStrategy}
  //  *
  //  * @param eventFactory to newInstance entries for filling the RingBuffer
  //  * @param bufferSize of the RingBuffer that will be rounded up to the next power of 2
  //  */
  // RingBuffer(const int bufferSize) {
  //       this(eventFactory,
  //            new MultiThreadedClaimStrategy(bufferSize),
  //            new BlockingWaitStrategy());
  // }
 

    // if (nullptr != defaultClaimStrategy_) { delete defaultClaimStrategy_; }
    // if (nullptr != defaultWaitStrategy_)  { delete defaultWaitStrategy_; }


  // /* ... */
  // MultiThreadedClaimStrategy* defaultClaimStrategy_;
  // BlockingWaitStrategy* defaultWaitStrategy_;


}

#endif /* __DISRUPTOR_RINGBUFFER_HPP__ */
