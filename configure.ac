AC_INIT([disruptor], [0.1], [lclark@2bdx.net])
AC_CONFIG_SRCDIR([src/])

AC_PROG_CXX
AC_PROG_CC

AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])
AM_SILENT_RULES([yes])

AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_MKDIR_P

AC_CHECK_HEADERS([thread chrono atomic vector])

AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_INT64_T
AC_HEADER_TIME
dnl AC_CXX_COMPILE_STDCXX_0X

# Checks for libraries.
AC_CHECK_LIB(stdc++, [main])
AC_CHECK_LIB(rt, [main])
AC_CHECK_LIB(gtest, [main])
AC_CHECK_LIB(gtest_main, [main])

AC_CHECK_FUNCS([gettimeofday])

AM_CXXFLAGS="$AM_CXXFLAGS -std=c++0x -O3 -g -Wall"
AM_LDFLAGS="$AM_LDFLAGS -pthread"
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_OUTPUT
