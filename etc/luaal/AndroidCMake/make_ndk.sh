#!/bin/sh

(cd cmake_ndk_build
	for d in */ ; do
		(cd "$d"
			make -j2
		)
	done
)


##cp -f ../AndroidCMake/Android.mk dist
#mkdir -p dist/scripts
#cp -f ../re.lua dist/scripts

