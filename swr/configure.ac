#  Copyright 2016-2017 Yuehang Wu.
#
#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  on the rights to use, copy, modify, merge, publish, distribute, sub
#  license, and/or sell copies of the Software, and to permit persons to whom
#  the Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice (including the next
#  paragraph) shall be included in all copies or substantial portions of the
#  Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
#  ADAM JACKSON BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
#  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
#  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Process this file with autoconf to produce a configure script

AC_PREREQ([2.60])
m4_define([swr_major_version], [0])
m4_define([swr_minor_version], [0])
m4_define([swr_micro_version], [1])
m4_define([swr_version], [swr_major_version.swr_minor_version.swr_micro_version])
AC_INIT([swr],[swr_version])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CXX

AM_INIT_AUTOMAKE([1.11 parallel-tests foreign no-dist-gzip dist-xz color-tests subdir-objects])

dnl compile with debug symbols
AC_ARG_ENABLE(debug,
		AS_HELP_STRING([--disable-debug]),
		[DEBUG="no"],
		[DEBUG="yes"])

if test "x$DEBUG" = "xyes"; then
	AC_DEFINE(ENABLE_DEBUG,1,[Enable all debugging])
	CFLAGS="-O0 -ggdb3"
	CXXFLAGS="-O0 -ggdb3 -std=c++11"
fi

AC_OUTPUT([
	Makefile
	src/Makefile
])
