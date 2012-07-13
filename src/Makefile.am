AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libvaront.la

libvaront_la_SOURCES = Sequencer.cpp Util.cpp

library_includedir = $(includedir)/varont
library_include_HEADERS = AbstractMultithreadedClaimStrategy.hpp			\
AggregateEventHandler.hpp AlertException.hpp BatchDescriptor.hpp			\
BatchEventProcessor.hpp BlockingWaitStrategy.hpp ClaimStrategy.hpp		\
EventFactory.hpp EventHandler.hpp EventProcessor.hpp									\
ExceptionHandler.hpp FatalExceptionHandler.hpp												\
IllegalStateException.hpp InsufficientCapacityException.hpp						\
LifecycleAwareEventHandler.hpp LifecycleAware.hpp											\
MultiThreadedClaimStrategy.hpp																				\
MultiThreadedLowContentionClaimStrategy.hpp MutableLong.hpp						\
NoOpEventProcessor.hpp PaddedLong.hpp ProcessingSequenceBarrier.hpp		\
RingBuffer.hpp SequenceBarrier.hpp Sequence.hpp Sequencer.hpp					\
SingleThreadedClaimStrategy.hpp SleepingWaitStrategy.hpp TimeUnit.hpp	\
Util.hpp WaitStrategy.hpp
