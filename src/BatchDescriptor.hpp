#ifndef __DISRUPTOR_BATCHDESCRIPTOR_HPP__
#define __DISRUPTOR_BATCHDESCRIPTOR_HPP__

#include "Sequencer.hpp"

namespace disruptor {

/**
 * Used to record the batch of sequences claimed via a {@link Sequencer}.
 */
class BatchDescriptor {
  const int size_;
  long end_;
public:
  /**
   * Create a holder for tracking a batch of claimed sequences in a {@link Sequencer}
   * @param size of the batch to claim.
   */
  BatchDescriptor(const int size)
    : size_(size)
    , end_(Sequencer::INITIAL_CURSOR_VALUE)
  {}

  /**
   * Get the end sequence of a batch.
   *
   * @return the end sequence in a batch
   */
  const long getEnd() const
  {
    return end_;
  }

  /**
   * Set the end of the batch sequence.  To be used by the {@link Sequencer}.
   *
   * @param end sequence in the batch.
   */
  void setEnd(const long end)
  {
    end_ = end;
  }

  /**
   * Get the size of the batch.
   *
   * @return the size of the batch.
   */
  const int getSize() const
  {
    return size_;
  }

  /**
   * Get the starting sequence for a batch.
   *
   * @return the starting sequence of a batch.
   */
  const long getStart() const
  {
    return end_ - (size_ - 1L);
  }

};

}

#endif /* __DISRUPTOR_BATCHDESCRIPTOR_HPP__ */
