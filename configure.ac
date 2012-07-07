AC_PREREQ([2.65])

# Version read as "major.minor[.revision]" and its use is subjective.
AC_INIT([disruptor], [0.1], [lclark@2bdx.net])
AC_CONFIG_SRCDIR([src/])

# Setting a default CFLAGS and CXXFLAGS, allowing override.
test ".$CFLAGS" = "." && CFLAGS="-O3 -g"
test ".$CXXFLAGS" = "." && CXXFLAGS="-O3 -g"

m4_include([m4/ax_check_compile_flag.m4])

AM_INIT_AUTOMAKE([1.10 -Wall no-define foreign])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_AWK
AC_PROG_MKDIR_P

AX_CHECK_COMPILE_FLAG([-std=c++0x] , [CXXFLAGS="$CXXFLAGS -std=c++0x"], [
  AC_MSG_ERROR(this project requires c++0x/c++11)])

AC_LANG_PUSH([C++])

AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_INT64_T
AC_HEADER_TIME

# Checks for libraries.
AC_CHECK_LIB(stdc++, [main])
AC_CHECK_LIB(rt, [main])
AC_CHECK_LIB(gtest, [main])
AC_CHECK_LIB(gtest_main, [main])

AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([test/Makefile])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([

  CFLAGS   = $CFLAGS
  CXXFLAGS = $CXXFLAGS

  LIBS = $LIBS

])
