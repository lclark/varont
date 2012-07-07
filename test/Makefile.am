AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

GTESTLIBS = -lgtest_main -lgtest -pthread
AM_CXXFLAGS := -I../src

TESTS = SequencerTest SingleThreadedClaimStrategyTest MultiThreadedClaimStrategyTest MultiThreadedLowContentionClaimStrategyTest CountDownLatchTest RingBufferTest LifecycleAwareTest SequenceBarrierTest BatchEventProcessorTest BatchPublisherTest AggregateEventHandlerTest

check_PROGRAMS = $(TESTS)
noinst_PROGRAMS = $(TESTS)

SequencerTest_SOURCES = SequencerTest.cpp
SequencerTest_LDADD = ../src/libvaront.la
SequencerTest_LDFLAGS = $(GTESTLIBS)

BatchEventProcessorTest_SOURCES = BatchEventProcessorTest.cpp
BatchEventProcessorTest_LDADD = ../src/libvaront.la
BatchEventProcessorTest_LDFLAGS = $(GTESTLIBS)

AggregateEventHandlerTest_SOURCES = AggregateEventHandlerTest.cpp
AggregateEventHandlerTest_LDADD = ../src/libvaront.la
AggregateEventHandlerTest_LDFLAGS = $(GTESTLIBS)

BatchPublisherTest_SOURCES = BatchPublisherTest.cpp
BatchPublisherTest_LDADD = ../src/libvaront.la
BatchPublisherTest_LDFLAGS = $(GTESTLIBS)

SequenceBarrierTest_SOURCES = SequenceBarrierTest.cpp
SequenceBarrierTest_LDADD = ../src/libvaront.la
SequenceBarrierTest_LDFLAGS = $(GTESTLIBS)

RingBufferTest_SOURCES = RingBufferTest.cpp
RingBufferTest_LDADD = ../src/libvaront.la
RingBufferTest_LDFLAGS = $(GTESTLIBS)

LifecycleAwareTest_SOURCES = LifecycleAwareTest.cpp
LifecycleAwareTest_LDADD = ../src/libvaront.la
LifecycleAwareTest_LDFLAGS = $(GTESTLIBS)

SingleThreadedClaimStrategyTest_SOURCES = SingleThreadedClaimStrategyTest.cpp
SingleThreadedClaimStrategyTest_LDADD = ../src/libvaront.la
SingleThreadedClaimStrategyTest_LDFLAGS = $(GTESTLIBS)

MultiThreadedClaimStrategyTest_SOURCES = MultiThreadedClaimStrategyTest.cpp
MultiThreadedClaimStrategyTest_LDADD = ../src/libvaront.la
MultiThreadedClaimStrategyTest_LDFLAGS = $(GTESTLIBS)

MultiThreadedLowContentionClaimStrategyTest_SOURCES = MultiThreadedLowContentionClaimStrategyTest.cpp
MultiThreadedLowContentionClaimStrategyTest_LDADD = ../src/libvaront.la
MultiThreadedLowContentionClaimStrategyTest_LDFLAGS = $(GTESTLIBS)


CountDownLatchTest_SOURCES = CountDownLatchTest.cpp
CountDownLatchTest_LDFLAGS = $(GTESTLIBS)
