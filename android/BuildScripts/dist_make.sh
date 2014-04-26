#!/bin/sh
# This must be run from the root directory of the project, not the subdirectory this script is contained in.

LATEST_ANDROID_NDK_ROOT=$ANDROID_NDK_ROOT

ROOT_DIR=`pwd`
BUILD_DIR=buildandroidcmake
TARGET_DIR=$BUILD_DIR/ALmixer
OPENAL_PACKAGE_DIR=$BUILD_DIR/openal-soft

if [ -z "$1" ]
then
	echo "No argument supplied. Assuming $OPENAL_ROOT_DIR is at ../openal-soft"
	OPENAL_ROOT_DIR=../openal-soft
else
	OPENAL_ROOT_DIR=$1
fi

"$ROOT_DIR/android/BuildScripts/build_module.sh" $OPENAL_ROOT_DIR

# Make the tarball, excluding the unnecessary build stuff
(cd $BUILD_DIR
tar -zcvf ALmixer-android.tar.gz --exclude='./ALmixer/armeabi'  --exclude='./ALmixer/armeabi-v7a' --exclude='./ALmixer/x86' ALmixer
)
#export NDK_MODULE_PATH=`pwd`/$TARGET_DIR


# Make the OpenAL-Soft tarball for convenience
mkdir -p "$OPENAL_PACKAGE_DIR"
rsync -avz "$OPENAL_ROOT_DIR/libs" "$OPENAL_PACKAGE_DIR"
rsync -avz "$OPENAL_ROOT_DIR/Android.mk" "$OPENAL_PACKAGE_DIR"
mkdir -p "$OPENAL_PACKAGE_DIR/jni/OpenAL/include/AL"
rsync -avz "$OPENAL_ROOT_DIR/jni/OpenAL/include/AL" "$OPENAL_PACKAGE_DIR/jni/OpenAL/include"
(cd $BUILD_DIR
tar -zcvf openalsoft-android.tar.gz openal-soft
)





