AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

GTESTLIBS = -lgtest_main -lgtest -pthread
AM_CXXFLAGS := @AM_CXXFLAGS@ -Wall -I../src

TESTS = SequencerTest SingleThreadedClaimStrategyTest MultiThreadedClaimStrategyTest MultiThreadedLowContentionClaimStrategyTest CountDownLatchTest RingBufferTest LifecycleAwareTest SequenceBarrierTest BatchEventProcessorTest BatchPublisherTest AggregateEventHandlerTest

check_PROGRAMS = $(TESTS)
noinst_PROGRAMS = $(TESTS)

SequencerTest_SOURCES = SequencerTest.cpp
SequencerTest_LDADD = ../src/libdisruptor.la
SequencerTest_LDFLAGS = $(GTESTLIBS)

BatchEventProcessorTest_SOURCES = BatchEventProcessorTest.cpp
BatchEventProcessorTest_LDADD = ../src/libdisruptor.la
BatchEventProcessorTest_LDFLAGS = $(GTESTLIBS)

AggregateEventHandlerTest_SOURCES = AggregateEventHandlerTest.cpp
AggregateEventHandlerTest_LDADD = ../src/libdisruptor.la
AggregateEventHandlerTest_LDFLAGS = $(GTESTLIBS)

BatchPublisherTest_SOURCES = BatchPublisherTest.cpp
BatchPublisherTest_LDADD = ../src/libdisruptor.la
BatchPublisherTest_LDFLAGS = $(GTESTLIBS)

SequenceBarrierTest_SOURCES = SequenceBarrierTest.cpp
SequenceBarrierTest_LDADD = ../src/libdisruptor.la
SequenceBarrierTest_LDFLAGS = $(GTESTLIBS)

RingBufferTest_SOURCES = RingBufferTest.cpp
RingBufferTest_LDADD = ../src/libdisruptor.la
RingBufferTest_LDFLAGS = $(GTESTLIBS)

LifecycleAwareTest_SOURCES = LifecycleAwareTest.cpp
LifecycleAwareTest_LDADD = ../src/libdisruptor.la
LifecycleAwareTest_LDFLAGS = $(GTESTLIBS)

SingleThreadedClaimStrategyTest_SOURCES = SingleThreadedClaimStrategyTest.cpp
SingleThreadedClaimStrategyTest_LDADD = ../src/libdisruptor.la
SingleThreadedClaimStrategyTest_LDFLAGS = $(GTESTLIBS)

MultiThreadedClaimStrategyTest_SOURCES = MultiThreadedClaimStrategyTest.cpp
MultiThreadedClaimStrategyTest_LDADD = ../src/libdisruptor.la
MultiThreadedClaimStrategyTest_LDFLAGS = $(GTESTLIBS)

MultiThreadedLowContentionClaimStrategyTest_SOURCES = MultiThreadedLowContentionClaimStrategyTest.cpp
MultiThreadedLowContentionClaimStrategyTest_LDADD = ../src/libdisruptor.la
MultiThreadedLowContentionClaimStrategyTest_LDFLAGS = $(GTESTLIBS)


CountDownLatchTest_SOURCES = CountDownLatchTest.cpp
CountDownLatchTest_LDFLAGS = $(GTESTLIBS)
