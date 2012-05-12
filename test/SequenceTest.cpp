#include <gtest/gtest.h>

struct SequenceTest : public testing::Test {
  const int BUFFER_SIZE = 4;

  Sequence gatingSequence = new Sequence(Sequencer.INITIAL_CURSOR_VALUE);
}
