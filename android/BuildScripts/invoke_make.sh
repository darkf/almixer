#!/bin/sh

# Simple helper script to invoke make on the already generated CMake projects.
# Also invokes ant.

./android/CMakeSupport/make_cmakeandroid.pl --targetdir=build "$@"
#export NDK_MODULE_PATH=`pwd`/build
#ant

