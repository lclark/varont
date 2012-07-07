varont (Varon-T)
================

A port of [disruptor](http://code.google.com/p/disruptor/) to C++.

The (incomplete) goal of this port is to replicate the functionality
of Disruptor as faithfully as possible.  The code structure is nearly
identical, as is the behavior, and some of the documentation within
the code has been copied verbatim.  For these reasons (and because
it's a fine choice) the Apache License 2.0 has been selected.

Building
--------

Requires C++0x/C++11 support.  Checks run successfully with GCC 4.6
and 4.7 (-std=c++0x).

Building and running the tests requires gtest (libgtest and libgtest_main).

    ./autogen.sh && ./configure && make check

status
------

9 of 16 unit tests are complete.  These tests cover most of the core
functionality (see TODO).

None of the performance tests are complete.

I expect problems related to my interpretation and usage of the [C++
atomic types and associated
functions](http://en.cppreference.com/w/cpp/atomic).  For example, the
intended behavior (as intended by Disruptor implementation) vs. actual
behavior of compare_exchange_strong within Sequence I do not yet fully
understand, and therefore have not vetted.

In other words, there be demons.

Usage / Examples
----------------

Not yet defined.

Performance
-----------

perf tests incomplete.

Varon-T Disruptor
-----------------

> "... banned ... due to its vicious nature"

[http://en.memory-alpha.org/wiki/Varon-T_disruptor](http://en.memory-alpha.org/wiki/Varon-T_disruptor)


Disruptor ["... was so named because it had elements of similarity for dealing with graphs of dependencies to the concept of "Phasers" in Java 7"][1].  The name Varon-T was selected as a continuation of this theme.

[1]: http://disruptor.googlecode.com/files/Disruptor-1.0.pdf  "Disruptor Technical Paper"
