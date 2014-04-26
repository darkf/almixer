#!/bin/sh
# This must be run from the root directory of the project, not the subdirectory this script is contained in.

LATEST_ANDROID_NDK_ROOT=$ANDROID_NDK_ROOT

ROOT_DIR=`pwd`
TARGET_DIR=buildandroidcmake/ALmixer

if [ -z "$1" ]
then
	echo "No argument supplied. Assuming $OPENAL_ROOT_DIR is at ../openal-soft"
	OPENAL_ROOT_DIR=../openal-soft
else
	OPENAL_ROOT_DIR=$1
fi

# To create
#git submodule add git@github.com:ewmailing/ALmixer.git ALmixer
#git submodule update --init
#git commit -m "Added ALmixer as submodule"

# to fetch (unverified)
#git submodule update --init


#Add copy of Android-CMake toolchain and helper Perl scripts for convenience.
# (Official repo is in Mercurial and I didn't want to add a git-hg dependency for demo purposes.)

mkdir -p $TARGET_DIR


# DO NOT SET ANDROID_NDK variable because CMake-Android script doesn't choose the standalone toolchain (bug).
#unset ANDROID_NDK

# The STLPort flag is not required for C code like Chipmunk. But don't forget it for C++ code.
./android/CMakeSupport/gen_cmakeandroid_almixer.pl --sourcedir="$ROOT_DIR" --targetdir="$TARGET_DIR" --toolchain=android/CMakeSupport/android.toolchain.cmake --standalone=$ANDROID_NDK_ROOT/standalone/ --openalbasedir=$OPENAL_ROOT_DIR --buildtype=MinSizeRel -DENABLE_ALMIXER_THREADS=1 --no-build

./android/CMakeSupport/make_cmakeandroid.pl --targetdir="$TARGET_DIR" -j2

mkdir -p "$TARGET_DIR/include"
# Module-ify the built libraries to conform to the Android external module system
rsync -avz ALmixer.h "$TARGET_DIR/include"
rsync -avz Isolated/ALmixer_RWops.h "$TARGET_DIR/include"
rsync -avz android/module/Android.mk "$TARGET_DIR"
#export NDK_MODULE_PATH=`pwd`/build

# (A lib directory needs to exist. Titanium bug in my opinion.)
#mkdir -p lib

#ant

# output goes to the dist subdirectory

