IF(APPLE)
	# Need @rpath support introduced in 2.8.12
	CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
ELSE()
	# Probably will work with 2.4
	CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENDIF()
# These are all useless <sigh>
cmake_policy(SET CMP0000 OLD)
cmake_policy(SET CMP0004 OLD)

PROJECT(ALmixer)

INCLUDE(CMakeDependentOption)
INCLUDE(CheckFunctionExists)
SET(CPACK_SET_DESTDIR TRUE)
INCLUDE(CPack)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})
FIND_PACKAGE(OpenAL)


OPTION(WANTS_BUILD_SHARED_LIBRARY "Set to ON to build dynamic library." ON)
OPTION(ALMIXER_OPTION_COMPILE_WITH_SDL "Compile ALmixer with dependencies on SDL" OFF)
OPTION(ALMIXER_OPTION_COMPILE_WITH_SDLSOUND "Compile ALmixer with dependencies on SDL_sound (requires COMPILE_WITH_SDL)" OFF)
OPTION(ENABLE_ALMIXER_THREADS "Use background thread for ALmixer_Update()" OFF)

# SDL implies SDL+SDL_sound. If we are not using SDL_sound, we need to supply our own decoders.
IF(NOT ALMIXER_OPTION_COMPILE_WITH_SDLSOUND)
	IF(ALMIXER_OPTION_COMPILE_WITH_SDL)
		FIND_PACKAGE(SDL2)
	ELSE()
		IF(APPLE)
			# Need Foundation for RWops backend because it touches NSFileManager, etc.
			FIND_LIBRARY(APPLE_FOUNDATION_LIBRARY Foundation)
			# Need QuartzCore for CACurrentMediaTime for ALmixer_GetTicks()
			FIND_LIBRARY(APPLE_QUARTZCORE_LIBRARY QuartzCore)
		ENDIF()
	ENDIF()
	# I can't think of a good reason not to use the native Core Audio decoder, but here's the option anyway.
	# Logic is inverted so the code will compile it in if the switch is not defined for those not going through CMake.
	IF(APPLE)
		OPTION(ALMIXER_DISABLE_COREAUDIO_DECODER "Disable Apple CoreAudio decoder which supports AAC,MP3,WAV,etc. (Usually a bad idea)" OFF)
		IF(NOT ALMIXER_DISABLE_COREAUDIO_DECODER)
			FIND_LIBRARY(APPLE_AUDIOTOOLBOX_LIBRARY AudioToolbox)
		ENDIF()
	ENDIF(APPLE)

	# Android's OpenSL ES decoder extensions support AAC, MP3, WAV, OGG, and others, but the APIs only exist starting in 4.0.
	# Additionally, their APIs are crap and they have no useful byte/streaming API so RWops can't work.
	# However, for most purposes, people won't need RWops and this is a very useful option as it can shrink binary size and save battery.
	IF(ANDROID)
		FIND_LIBRARY( ANDROID_LOG_LIBRARY log )
		OPTION(ALMIXER_USE_ANDROID_OPENSLES_DECODER "Use native Android OpenSLES decoder which supports AAC,MP3,WAV,etc. Requires 4.0+" ON)
		IF(ALMIXER_USE_ANDROID_OPENSLES_DECODER)
			FIND_LIBRARY( ANDROID_ANDROID_LIBRARY android )
			FIND_LIBRARY( OPENSLES_LIBRARY OpenSLES )
		ENDIF()
	ENDIF(ANDROID)

	# Windows Media Foundation supports AAC, MP3, WAV, and others, but the APIs only exist starting in Vista.
	# Additionally, it is unclear if this actually works in Vista even though the APIs were introduced there.
	# Windows 7 and greater are the only ones guaranteed to work.
	# (Microsoft has dropped XP support as of this writing, and the lion share of installations is Windows 7, 
	# so making this the default seems reasonable now. We can't get AAC support any other way.)
	IF(WIN32)
		OPTION(ALMIXER_USE_WINDOWS_MEDIA_FOUNDATION_DECODER "Use native Windows Media Foundation decoder which supports AAC,MP3,WAV,etc. Requires Win7+" ON)
	ENDIF(WIN32)

	# These systems provide native decoders, so we don't need these others by default. They will bloat binary size and eat battery.
	IF(APPLE OR ANDROID OR WIN32)
		OPTION(ALMIXER_USE_OGG_VORBIS_DECODER "Use Ogg Vorbis decoder (LGPL)" OFF)
		OPTION(ALMIXER_USE_OGG_TREMOR_DECODER "Use Ogg Tremor decoder (LGPL)" OFF)
		OPTION(ALMIXER_USE_WAV_DECODER "Use WAV decoder (LGPL)" OFF)
		OPTION(ALMIXER_USE_MPG123_DECODER "Use MPG123 decoder (LGPL)" OFF)
	ELSE()
		# FIXME: Should have way to reject picking both vorbis and tremor at the same time.
		OPTION(ALMIXER_USE_OGG_VORBIS_DECODER "Use Ogg Vorbis decoder (LGPL)" ON)
		# Rough guess that this is not a mobile system, so prefer Vorbis
		OPTION(ALMIXER_USE_OGG_TREMOR_DECODER "Use Ogg Tremor decoder (LGPL)" OFF)
		OPTION(ALMIXER_USE_WAV_DECODER "Use WAV decoder (LGPL)" ON)
		# Looks like distributions are avoiding bundling this by default, probably because of patent worries.
		OPTION(ALMIXER_USE_MPG123_DECODER "Use MPG123 decoder (LGPL)" OFF)
	ENDIF()
ELSE()
	FIND_PACKAGE(SDL2)
	# Note: SDL_sound needs to be updated for 2.6+. You'll get annoying warnings.
	FIND_PACKAGE(SDL_sound)
ENDIF()

SET(ALMIXER_MAJOR_VERSION 0)
SET(ALMIXER_MINOR_VERSION 4)
SET(ALMIXER_PATCH_VERSION 0)

SET(ALMIXER_VERSION ${ALMIXER_MAJOR_VERSION}.${ALMIXER_MINOR_VERSION}.${ALMIXER_PATCH_VERSION})
SET(ALMIXER_COMPATIBILITY_VERSION ${ALMIXER_MAJOR_VERSION}.${ALMIXER_MINOR_VERSION}.0)
# ??? Don't know
SET(ALMIXER_SOVERSION ${ALMIXER_MAJOR_VERSION}.${ALMIXER_MINOR_VERSION}.0)



IF(APPLE)
	CMAKE_DEPENDENT_OPTION(WANTS_BUILD_FRAMEWORK "Set to ON to build framework instead of dylib. Only valid if BUILD_SHARED_LIBRARY is ON an is OS X." ON "WANTS_BUILD_SHARED_LIBRARY" ON)
ENDIF(APPLE)

# FIXME: This kind of assumes Android is always using the Apportable Android.
IF(ANDROID)
	OPTION(ALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS "Uses alcSuspend/alcResume extensions to stop CPU processing while backgrounded." ON)
ENDIF(ANDROID)


# RPATH stuff. This MUST be set before any targets that use it are declared.
IF("${CMAKE_SYSTEM}" MATCHES "Linux" AND NOT ANDROID)
	# RPATH $ORIGIN is only for Linux and Solaris
	# FIXME: match for Solaris too

	# use, i.e. don't skip the full RPATH for the build tree
	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

	#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


	# the RPATH to be used when installing, but only if it's not a system directory
	LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
	IF("${isSystemDir}" STREQUAL "-1")
	#   SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	#    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:\\$$ORIGIN")
	ENDIF("${isSystemDir}" STREQUAL "-1")
	    SET(CMAKE_INSTALL_RPATH "\\\$ORIGIN")
	#    SET(CMAKE_INSTALL_RPATH "/usr/lib${CMAKE_INSTALL_RPATH}:\\\$ORIGIN")
ELSEIF(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	# use, i.e. don't skip the full RPATH for the build tree
	#	SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

	# when building, don't use the install RPATH already
	# (but later on when installing)
	SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

	#SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)	
ENDIF()

SET(ALMIXER_LIBRARY_NAME ALmixer)
IF(APPLE)
	SET(CMAKE_FRAMEWORK_INSTALL_DIR "/Library/Frameworks" CACHE STRING "Directory to install frameworks to.")
	# @rpath now replaces this
#	SET(CMAKE_FRAMEWORK_INSTALL_NAME_DIR "@executable_path/../Frameworks" CACHE STRING "install_name path for framework.")
	#SET(CMAKE_DYLIB_INSTALL_NAME_DIR "" CACHE STRING "install_name path for dylib.")
	SET(CMAKE_DYLIB_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "install_name path for dylib.")
	SET(ALMIXER_FRAMEWORK_NAME "${ALMIXER_LIBRARY_NAME}.framework")
	SET(ALMIXER_FRAMEWORK_VERSION_NUMBER "A")
	SET(ALMIXER_FRAMEWORK_VERSIONED_EXECUTABLE_DIR "Versions/${ALMIXER_FRAMEWORK_VERSION_NUMBER}/MacOS")
	SET(ALMIXER_FRAMEWORK_VERSIONED_LIB_DIR "Versions/${ALMIXER_FRAMEWORK_VERSION_NUMBER}/lib")
	SET(ALMIXER_FRAMEWORK_CURRENT_EXECUTABLE_DIR "Versions/Current/MacOS")

	# For Apple install_name, is it better to detect if Xcode vs Makefile?
	# Xcode default=1, Makefile=0? Or detect if Framework vs. dylib,
	# Framework=1, dylib=0?
	OPTION(CMAKE_BUILD_WITH_INSTALL_RPATH "Set to YES to set the rpath or install_name on build instead of install." ON)

ELSEIF(UNIX)
	SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "rpaths separated by semicolons.")
	OPTION(CMAKE_BUILD_WITH_INSTALL_RPATH "Set to YES to set the rpath or install_name on build instead of install." OFF)

ELSEIF(WIN32)

ENDIF()



SET(ALMIXER_CORE_SOURCE
	${ALmixer_SOURCE_DIR}/src/ALmixer.c
	${ALmixer_SOURCE_DIR}/src/CircularQueue.c
	${ALmixer_SOURCE_DIR}/src/CircularQueue.h
	${ALmixer_SOURCE_DIR}/src/LinkedList.c
	${ALmixer_SOURCE_DIR}/src/LinkedList.h
	${ALmixer_SOURCE_DIR}/src/tErrorLib.c
	${ALmixer_SOURCE_DIR}/src/tErrorLib.h
)
IF(ANDROID)
	SET(ALMIXER_CORE_SOURCE
		${ALMIXER_CORE_SOURCE}
		${ALmixer_SOURCE_DIR}/src/ALmixer_AndroidExtensions.c
		${ALmixer_SOURCE_DIR}/src/ALmixer_AndroidExtensions.h
	)
ENDIF()

SET(ALMIXER_WITHOUT_SDL_SOURCE
	${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleThread.h
	${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleMutex.h
	${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleMutex.c
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/file/ALmixer_RWops.c
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/stdlib/ALmixer_malloc.c
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/stdlib/ALmixer_stdlib.c
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/stdlib/ALmixer_string.c
	# Note: Currently #ifdef'd only for Windows to save space since only Windows needs it.
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/stdlib/ALmixer_iconv.c
	# Note: Currently #ifdef'd only for Windows to save space since only Windows needs it.
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/stdlib/ALmixer_getenv.c
)
SET(ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS
	${ALmixer_SOURCE_DIR}/src/StandAlone
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/include
	${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src
)

IF(APPLE)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/file/cocoa/ALmixer_rwopsbundlesupport.h
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/file/cocoa/ALmixer_rwopsbundlesupport.m
	)
	SET(ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS
		${ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/file/cocoa
	)
ELSEIF(ANDROID)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/android/ALmixer_android.c
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/android/ALmixer_android.h
	)
	SET(ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS
		${ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/android
	)
ELSEIF(WIN32)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/windows/ALmixer_windows.c
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/windows/ALmixer_windows.h
	)
	SET(ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS
		${ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SDL/src/core/windows
	)
ENDIF()

SET(PUBLIC_HEADERS
	${ALmixer_SOURCE_DIR}/include/ALmixer.h
	${ALmixer_SOURCE_DIR}/include/ALmixer_RWops.h
	${ALmixer_SOURCE_DIR}/include/ALmixer_PlatformExtensions.h
)

SET(SOUNDDECODER_SOURCE
	${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/SoundDecoder.c
	${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/SoundDecoder.h
	${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/SoundDecoder_Internal.h
)

SET(SOUNDDECODER_INCLUDE_PATHS
	${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder
)



IF(APPLE)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleThreadPosix.c
	)
ELSEIF(ANDROID)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleThreadPosix.c
		
	)
ELSEIF(UNIX)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleThreadPosix.c
	)
ELSEIF(WIN32)
	SET(ALMIXER_WITHOUT_SDL_SOURCE
		${ALMIXER_WITHOUT_SDL_SOURCE}
		${ALmixer_SOURCE_DIR}/src/StandAlone/SimpleThreadWindows.c
	)
ENDIF()

SET(ALMIXER_SELECTED_SOURCE_DECODERS "")
IF(APPLE AND NOT ALMIXER_DISABLE_COREAUDIO_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/coreaudio.c
	)
ELSEIF(ANDROID AND ALMIXER_USE_ANDROID_OPENSLES_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/OpenSLES_Android.c
	)
ELSEIF(WIN32 AND ALMIXER_USE_WINDOWS_MEDIA_FOUNDATION_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/WindowsMediaFoundation.cpp
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/WindowsMediaFoundation_IMFByteStreamRWops.cpp
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/WindowsMediaFoundation_IMFByteStreamRWops.hpp
	)
ENDIF()

IF(ALMIXER_USE_OGG_VORBIS_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/LGPL/oggvorbis.c
	)
ELSEIF(ALMIXER_USE_OGG_TREMOR_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/LGPL/oggtremor.c
	)
ENDIF()

IF(ALMIXER_USE_WAV_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/LGPL/SDL_sound_minimal.c
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/LGPL/wav.c
	)
ENDIF()
IF(ALMIXER_USE_MPG123_DECODER)
	SET(ALMIXER_SELECTED_SOURCE_DECODERS
		${ALMIXER_SELECTED_SOURCE_DECODERS} 
		${ALmixer_SOURCE_DIR}/src/StandAlone/SoundDecoder/LGPL/mpg123.c
	)
ENDIF()


IF(NOT ALMIXER_OPTION_COMPILE_WITH_SDLSOUND)

	IF(ALMIXER_OPTION_COMPILE_WITH_SDL)
		ADD_DEFINITIONS(-DALMIXER_COMPILE_WITH_SDL)
		SET(ALMIXER_FILES
			${ALMIXER_CORE_SOURCE}
			${SOUNDDECODER_SOURCE}
			${ALMIXER_SELECTED_SOURCE_DECODERS}
		)
	ELSE()
		REMOVE_DEFINITIONS(-DALMIXER_COMPILE_WITH_SDL)
		SET(ALMIXER_FILES
			${ALMIXER_CORE_SOURCE}
			${ALMIXER_WITHOUT_SDL_SOURCE}
			${SOUNDDECODER_SOURCE}
			${ALMIXER_SELECTED_SOURCE_DECODERS}
		)
	ENDIF()	

#	MESSAGE("ALMIXER_FILES ${ALMIXER_FILES}")

	CHECK_FUNCTION_EXISTS(vasprintf HAVE_VASPRINTF)
	IF(NOT HAVE_VASPRINTF)
		ADD_DEFINITIONS(-DDONT_USE_VASPRINTF)
	ENDIF()
	# Temporary. Should only define if no native decoder exists.
	#ADD_DEFINITIONS(-DSOUND_SUPPORTS_WAV)

	IF(APPLE AND NOT ALMIXER_DISABLE_COREAUDIO_DECODER)
		REMOVE_DEFINITIONS(-DSOUND_DISABLE_COREAUDIO)
	ELSE()
		ADD_DEFINITIONS(-DSOUND_DISABLE_COREAUDIO)
	ENDIF()
	
	IF(ANDROID AND ALMIXER_USE_ANDROID_OPENSLES_DECODER)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_ANDROID_OPENSLES)
	ELSE()
		REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_ANDROID_OPENSLES)
	ENDIF()
		
	IF(WIN32 AND ALMIXER_USE_WINDOWS_MEDIA_FOUNDATION_DECODER)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_WINDOWSMEDIAFOUNDATION)
	ELSE()
		REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_WINDOWSMEDIAFOUNDATION)
	ENDIF()
	
	IF(ALMIXER_USE_OGG_VORBIS_DECODER)
		FIND_PACKAGE(Ogg)
		FIND_PACKAGE(Vorbis)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_OGG)
	ELSEIF(ALMIXER_USE_OGG_TREMOR_DECODER)
		FIND_PACKAGE(Ogg)
		FIND_PACKAGE(Tremor)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_OGG)
	ELSE()
		REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_OGG)
	ENDIF()

	IF(ALMIXER_USE_WAV_DECODER)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_WAV)
	ELSE()
		REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_WAV)
	ENDIF()
	
	IF(ALMIXER_USE_MPG123_DECODER)
		FIND_PACKAGE(MPG123)
		ADD_DEFINITIONS(-DSOUND_SUPPORTS_MPG123)
	ELSE()
		REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_MPG123)
	ENDIF()
	


ELSE()
	SET(ALMIXER_FILES
		${ALMIXER_CORE_SOURCE}
	)
	REMOVE_DEFINITIONS(-DALMIXER_COMPILE_WITH_SDLSOUND)
	REMOVE_DEFINITIONS(-DSOUND_DISABLE_COREAUDIO)
	REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_ANDROID_OPENSLES)
	REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_WINDOWSMEDIAFOUNDATION)
	REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_OGG)
	REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_WAV)
	REMOVE_DEFINITIONS(-DSOUND_SUPPORTS_MPG123)
	IF(ALMIXER_OPTION_COMPILE_WITH_SDL)
		ADD_DEFINITIONS(-DALMIXER_COMPILE_WITH_SDL)
	ELSE()
		REMOVE_DEFINITIONS(-DALMIXER_COMPILE_WITH_SDL)
	ENDIF()	
ENDIF()


#SET(RESOURCE_FILES
#)

ADD_DEFINITIONS(-DALMIXER_BUILD_LIBRARY)
IF(ENABLE_ALMIXER_THREADS)
	ADD_DEFINITIONS(-DENABLE_ALMIXER_THREADS)
ELSE(ENABLE_ALMIXER_THREADS)
	REMOVE_DEFINITIONS(-DENABLE_ALMIXER_THREADS)
ENDIF(ENABLE_ALMIXER_THREADS)

# FIXME: This kind of assumes Android is always using the Apportable Android.
IF(ANDROID)
	IF(ALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS)
		ADD_DEFINITIONS(-DALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS)
	ELSE(ALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS)
		REMOVE_DEFINITIONS(-DALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS)
	ENDIF(ALMIXER_USE_APPORTABLE_OPENAL_EXTENSIONS)
ENDIF(ANDROID)



IF(WANTS_BUILD_SHARED_LIBRARY)
	#ADD_LIBRARY(ALmixer SHARED ${PUBLIC_HEADERS} ${ALMIXER_FILES} ${RESOURCE_FILES})
	ADD_LIBRARY(ALmixer SHARED ${PUBLIC_HEADERS} ${ALMIXER_FILES})
ELSE(WANTS_BUILD_SHARED_LIBRARY)
	ADD_LIBRARY(ALmixer STATIC ${PUBLIC_HEADERS} ${ALMIXER_FILES} ${RESOURCE_FILES})
ENDIF(WANTS_BUILD_SHARED_LIBRARY)
#SET_TARGET_PROPERTIES(ALmixer PROPERTIES OUTPUT_NAME "ALmixer")

# Set Dynamic Library and Framework properties
IF(WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)

	SET_TARGET_PROPERTIES(ALmixer PROPERTIES
		FRAMEWORK TRUE
		FRAMEWORK_VERSION "${ALMIXER_FRAMEWORK_VERSION_NUMBER}"
		#  PRIVATE_HEADER "fooPrivate.h;fooBoth.h"
		PUBLIC_HEADER "${PUBLIC_HEADERS}"
		RESOURCE "${RESOURCE_FILES}"
		# INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}
		# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
		VERSION ${ALMIXER_VERSION}
		SOVERSION ${ALMIXER_COMPATIBILITY_VERSION}
#		COMPILE_FLAGS "${ALMIXER_C_FLAGS}"
	)
	
	# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
	# SET_TARGET_PROPERTIES will fail if the variable is empty.
	SET_PROPERTY(TARGET ALmixer PROPERTY INSTALL_NAME_DIR ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR})
	SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH NO)
	# If the user deletes the install_name path, use the gcc default
	# and disable the option completely. But CMake by default places
	# the name of the library in the install_name if 
	# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
	# disable the switch.
	IF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
		#		MESSAGE("got dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
		SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})

	ELSE(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)
		#		MESSAGE("no dir ${CMAKE_FRAMEWORK_INSTALL_NAME_DIR}")
		SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH NO)

	ENDIF(CMAKE_FRAMEWORK_INSTALL_NAME_DIR)



	# Short Version is the "marketing version". It is the version
	# the user sees in an information panel.
	SET(MACOSX_FRAMEWORK_SHORT_VERSION_STRING "${ALMIXER_MAJOR_VERSION}.${ALMIXER_MINOR_VERSION}.${ALMIXER_PATCH_VERSION}")
	# Bundle version is the version the OS looks at.
	SET(MACOSX_FRAMEWORK_BUNDLE_VERSION "${ALMIXER_MAJOR_VERSION}.${ALMIXER_MINOR_VERSION}.${ALMIXER_PATCH_VERSION}")
	SET(MACOSX_FRAMEWORK_IDENTIFIER "net.playcontrol.almixer")

ELSEIF(WANTS_BUILD_SHARED_LIBRARY)
	# Need quotes around ${PUBLIC_HEADERS}, 
	# otherwise CMake only seems to use the first one
	SET_TARGET_PROPERTIES(ALmixer PROPERTIES
		PUBLIC_HEADER "${PUBLIC_HEADERS}"
	)
	IF(APPLE)
		SET_TARGET_PROPERTIES(ALmixer PROPERTIES
			FRAMEWORK FALSE
			# INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR}
			# BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
			VERSION ${ALMIXER_VERSION}
			SOVERSION ${ALMIXER_COMPATIBILITY_VERSION}
#			COMPILE_FLAGS "${ALMIXER_C_FLAGS}"
		)
		# I moved the INSTALL_NAME_DIR to use SET_PROPERTY instead because
		# SET_TARGET_PROPERTIES will fail if the variable is empty.
		SET_PROPERTY(TARGET ALmixer PROPERTY INSTALL_NAME_DIR ${CMAKE_DYLIB_INSTALL_NAME_DIR})
		SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH NO)
		# If the user deletes the install_name path, use the gcc default
		# and disable the option completely. But CMake by default places
		# the name of the library in the install_name if 
		# BUILD_WITH_INSTALL_RPATH is ON. So to avoid this, I need to 
		# disable the switch.
		IF(CMAKE_DYLIB_INSTALL_NAME_DIR)
			SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH})
		ELSE(CMAKE_DYLIB_INSTALL_NAME_DIR)
			SET_PROPERTY(TARGET ALmixer PROPERTY BUILD_WITH_INSTALL_RPATH OFF)
		ENDIF(CMAKE_DYLIB_INSTALL_NAME_DIR)


	ELSEIF(ANDROID)
		# Setting versions on Android breaks things because the Java System.loadLibrary can't handle the symlink/version convention.
		# Avoid setting version stuff.

	ELSEIF(UNIX)
		SET_TARGET_PROPERTIES(ALmixer PROPERTIES
			INSTALL_RPATH ${CMAKE_INSTALL_RPATH}
			BUILD_WITH_INSTALL_RPATH ${CMAKE_BUILD_WITH_INSTALL_RPATH} 
			VERSION ${ALMIXER_VERSION}
			SOVERSION ${ALMIXER_SOVERSION}
#			COMPILE_FLAGS "${ALMIXER_C_FLAGS}"
		)
	
	ELSEIF(WIN32)
		#		SET_TARGET_PROPERTIES(ALmixer PROPERTIES
		#	VERSION ${ALMIXER_VERSION}
		#	SOVERSION ${ALMIXER_COMPATIBILITY_VERSION}
		#)

	ELSE(APPLE)
		SET_TARGET_PROPERTIES(ALmixer PROPERTIES
			VERSION ${ALMIXER_VERSION}
			SOVERSION ${ALMIXER_COMPATIBILITY_VERSION}
#			COMPILE_FLAGS "${ALMIXER_C_FLAGS}"
		)

	ENDIF(APPLE)


ENDIF(WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)

IF(ALMIXER_OPTION_COMPILE_WITH_SDLSOUND)
	INCLUDE_DIRECTORIES(${ALmixer_SOURCE_DIR}/include ${ALmixer_SOURCE_DIR}/src ${OPENAL_INCLUDE_DIR} ${SDL_INCLUDE_DIR} ${SDL_SOUND_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(ALmixer ${OPENAL_LIBRARY} ${SDL_SOUND_LIBRARIES} ${SDL_LIBRARY}) 
ELSE()
	SET(ALMIXER_SELECTED_INCLUDE_PATHS "")
	SET(ALMIXER_SELECTED_LINKED_LIBRARIES "")
	IF(APPLE)
		IF(NOT ALMIXER_OPTION_COMPILE_WITH_SDL)
			SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${APPLE_FOUNDATION_LIBRARY} ${APPLE_QUARTZCORE_LIBRARY})
		ENDIF()

		IF(NOT ALMIXER_DISABLE_COREAUDIO_DECODER)
			TARGET_LINK_LIBRARIES(ALmixer ${OPENAL_LIBRARY} ${APPLE_AUDIOTOOLBOX_LIBRARY}) 
			SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${APPLE_AUDIOTOOLBOX_LIBRARY})
		ENDIF()
	ELSEIF(ANDROID)
		IF(ALMIXER_USE_ANDROID_OPENSLES_DECODER)
			SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${OPENSLES_LIBRARY} ${ANDROID_ANDROID_LIBRARY})
		ENDIF()
	ELSEIF(WIN32)
		IF(ALMIXER_USE_WINDOWS_MEDIA_FOUNDATION_DECODER)
			#Experimentally, I found that I don't need mf, evr for linking
			#But I don't know if that affects backwards compatibility (e.g. Vista)
			#evr.lib;mf.lib;mfplat.lib;mfreadwrite.lib;mfuuid.lib;
			#FIND_LIBRARY(WINDOWS_MEDIAFOUNDATION_MF_LIBRARY mf)
			#FIND_LIBRARY(WINDOWS_MEDIAFOUNDATION_MFPLAT_LIBRARY mfplat)
			#FIND_LIBRARY(WINDOWS_MEDIAFOUNDATION_MFREADWRITE_LIBRARY mfreadwrite)
			#FIND_LIBRARY(WINDOWS_MEDIAFOUNDATION_MFUUID_LIBRARY mfuuid)
			#FIND_LIBRARY(WINDOWS_MEDIAFOUNDATION_EVR_LIBRARY evr)
			SET(WINDOWS_MEDIAFOUNDATION_MF_LIBRARY mf.lib)
			SET(WINDOWS_MEDIAFOUNDATION_MFPLAT_LIBRARY mfplat.lib)
			SET(WINDOWS_MEDIAFOUNDATION_MFREADWRITE_LIBRARY mfreadwrite.lib)
			SET(WINDOWS_MEDIAFOUNDATION_MFUUID_LIBRARY mfuuid.lib)
			SET(WINDOWS_MEDIAFOUNDATION_EVR_LIBRARY evr.lib)

			SET(ALMIXER_SELECTED_LINKED_LIBRARIES
				${ALMIXER_SELECTED_LINKED_LIBRARIES} 
				${WINDOWS_MEDIAFOUNDATION_MF_LIBRARY}
				${WINDOWS_MEDIAFOUNDATION_MFPLAT_LIBRARY}
				${WINDOWS_MEDIAFOUNDATION_MFREADWRITE_LIBRARY}
				${WINDOWS_MEDIAFOUNDATION_MFUUID_LIBRARY}
				${WINDOWS_MEDIAFOUNDATION_EVR_LIBRARY}	
			)
		ELSE()
		
		ENDIF()
	ENDIF()
	
	# You can't use both Ogg Vorbis and Ogg Tremor. It is one or the other.
	IF(ALMIXER_USE_OGG_VORBIS_DECODER)
		SET(ALMIXER_SELECTED_INCLUDE_PATHS ${ALMIXER_SELECTED_INCLUDE_PATHS} ${VORBIS_INCLUDE_DIR} ${OGG_INCLUDE_DIR})
		SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${VORBIS_LIBRARIES} ${OGG_LIBRARY})
	ELSEIF(ALMIXER_USE_OGG_TREMOR_DECODER)
		SET(ALMIXER_SELECTED_INCLUDE_PATHS ${ALMIXER_SELECTED_INCLUDE_PATHS} ${TREMOR_INCLUDE_DIR} ${OGG_INCLUDE_DIR})
		SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${TREMOR_LIBRARY} ${OGG_LIBRARY})
	ENDIF()
	
	IF(ALMIXER_USE_MPG123_DECODER)
		SET(ALMIXER_SELECTED_INCLUDE_PATHS ${ALMIXER_SELECTED_INCLUDE_PATHS} ${MPG123_INCLUDE_DIR})		
		SET(ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES} ${MPG123_LIBRARY})
	ENDIF()
	
	IF(ALMIXER_OPTION_COMPILE_WITH_SDL)
		INCLUDE_DIRECTORIES(
			${OPENAL_INCLUDE_DIR}
			${ALmixer_SOURCE_DIR}/include
			${ALmixer_SOURCE_DIR}/src
			${ALMIXER_SELECTED_INCLUDE_PATHS}
			${SDL_INCLUDE_DIR}
		)

		IF(ANDROID)
			TARGET_LINK_LIBRARIES(ALmixer 
				${ALMIXER_SELECTED_LINKED_LIBRARIES}
				${OPENAL_LIBRARY}
				${ANDROID_LOG_LIBRARY}
				${SDL_LIBRARY}
			)
		ELSE()
			TARGET_LINK_LIBRARIES(ALmixer 
				${ALMIXER_SELECTED_LINKED_LIBRARIES}
				${OPENAL_LIBRARY}
				${SDL_LIBRARY}				
			)
		ENDIF()
	ELSE()
		INCLUDE_DIRECTORIES(
			${OPENAL_INCLUDE_DIR} 
			${ALmixer_SOURCE_DIR}/include
			${ALmixer_SOURCE_DIR}/src
			${SOUNDDECODER_INCLUDE_PATHS}
			${ALMIXER_WITHOUT_SDL_SOURCE_INCLUDE_PATHS}
			${ALMIXER_SELECTED_INCLUDE_PATHS}
		)

		IF(ANDROID)
			TARGET_LINK_LIBRARIES(ALmixer 
				${ALMIXER_SELECTED_LINKED_LIBRARIES}
				${OPENAL_LIBRARY}
				${ANDROID_LOG_LIBRARY}
			)
		ELSE()
			TARGET_LINK_LIBRARIES(ALmixer 
				${ALMIXER_SELECTED_LINKED_LIBRARIES}
				${OPENAL_LIBRARY}
			)
		ENDIF()

	ENDIF()

#	MESSAGE("ALMIXER_SELECTED_LINKED_LIBRARIES ${ALMIXER_SELECTED_LINKED_LIBRARIES}")

ENDIF()


# For Doxygen
# This will find the Doxygen stuff on your system if you want it.
# I need to use my version because the official version doesn't 
# understand the OS X version of Doxygen. It also doesn't define
# DOT_PATH which is a problem if dot is not in the path.
INCLUDE(${CMAKE_ROOT}/Modules/Documentation.cmake OPTIONAL)
#INCLUDE(${PROJECT_SOURCE_DIR}/CMakeModules/Documentation.cmake OPTIONAL)
OPTION(BUILD_DOCUMENTATION "Build Almixer reference documentation using doxygen (use: make DoxygenDoc)" OFF)
MARK_AS_ADVANCED(CLEAR BUILD_DOCUMENTATION)
# To build the documention, you will have to enable it
# and then do the equivalent of "make DoxygenDoc".
IF(BUILD_DOCUMENTATION)

    OPTION(BUILD_REF_DOCS_SEARCHENGINE "Enable doxygen's search engine (requires that documentation to be installed on a php enabled web server)" OFF)
    IF(BUILD_REF_DOCS_SEARCHENGINE)
        SET(SEARCHENGINE YES)
    ELSE()
        SET(SEARCHENGINE NO)
    ENDIF()

    OPTION(BUILD_REF_DOCS_TAGFILE "Generate a tag file named osg.tag on the documentation web server" OFF)
    IF(BUILD_REF_DOCS_TAGFILE)
		SET(GENERATE_TAGFILE "${PROJECT_BINARY_DIR}/doc/ALmixerDocumentation/ALmixer.tag")
    ELSE()
        SET(GENERATE_TAGFILE "")
    ENDIF()

    IF(DOT)
        SET(HAVE_DOT YES)
    ELSE()
        SET(HAVE_DOT NO)
    ENDIF()
    
    # If html help generation was requested. DOCUMENTATION_HTML_HELP is defined by Documentation.cmake
    SET(GENERATE_HTMLHELP "NO")
    IF(DOCUMENTATION_HTML_HELP)
        # on windows Documentation.cmake finds the html help workshop fi it exists. On u*ix we might have it with wine but no way to point it out
        IF(NOT WIN32)
            SET(HTML_HELP_COMPILER "" CACHE FILEPATH "Enter location of the HTML help compiler to let doxygen compile html")
            MARK_AS_ADVANCED(HTML_HELP_COMPILER)
        ENDIF()
        # this var sets a proper value in .doxygen files when coniguring them below
        SET(GENERATE_HTMLHELP "YES")
    endif()
  
    # This processes our doxyfile.cmake and substitutes paths to generate
    # a final Doxyfile
	CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/doc/ALmixer.doxyfile.cmake
		${PROJECT_BINARY_DIR}/doc/ALmixer.doxyfile
    )
# INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/doc/ALmixerDocumentation DESTINATION doc COMPONENT ALmixer-doc)


    # This creates a new target to build documentation.
    # It runs ${DOXYGEN} which is the full path and executable to
    # Doxygen on your system, set by the FindDoxygen.cmake module
    # (called by FindDocumentation.cmake).
    # It runs the final generated Doxyfile against it.
    # The DOT_PATH is substituted into the Doxyfile.
    ADD_CUSTOM_TARGET(DoxygenDoc ${DOXYGEN}
        ${PROJECT_BINARY_DIR}/doc/ALmixer.doxyfile
    )
ENDIF(BUILD_DOCUMENTATION)




# Install commands below
IF(APPLE AND WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)
	# Will install framework to /Library/Frameworks directory or user specified
	INSTALL(TARGETS
		ALmixer
		FRAMEWORK DESTINATION ${CMAKE_FRAMEWORK_INSTALL_DIR}
	)
ELSE(APPLE AND WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)
	INSTALL(TARGETS
		ALmixer
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		PUBLIC_HEADER DESTINATION include
	)
ENDIF(APPLE AND WANTS_BUILD_SHARED_LIBRARY AND WANTS_BUILD_FRAMEWORK)





# For uninstall (needs cmake_uninstall.cmake.in in the top-level directory)
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")


# The Examples
ADD_SUBDIRECTORY(EXAMPLES)



# Set defaults for Universal Binaries. We want 32-bit Intel/PPC on 10.4 
# and 32/64-bit Intel/PPC on >= 10.5. Anything <= 10.3 doesn't support.
IF(APPLE)
	# These are just defaults/recommendations, but how we want to build
	# out of the box. But the user needs to be able to change these options.
	# So we must only set the values the first time CMake is run, or we 
	# will overwrite any changes the user sets.
	# FORCE is used because the options are not reflected in the UI otherwise.
	# Seems like a good place to add version specific compiler flags too.
	IF(NOT ALMIXER_CONFIG_HAS_BEEN_RUN_BEFORE)
		# This is really fragile, but CMake doesn't provide the OS system 
		# version information we need. (Darwin versions can be changed 
		# independently of OS X versions.)
		# It does look like CMake handles the CMAKE_OSX_SYSROOT automatically.
#		IF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
#			SET(CMAKE_OSX_ARCHITECTURES "ppc;i386;x86_64" CACHE STRING "Build architectures for OSX" FORCE)
#			SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.5" CACHE STRING "Flags used by the compiler during all build types." FORCE)
#		ELSE(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
#			IF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#				SET(CMAKE_OSX_ARCHITECTURES "ppc;i386" CACHE STRING "Build architectures for OSX" FORCE)
#				SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.4" CACHE STRING "Flags used by the compiler during all build types." FORCE)
#			ELSE(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
				# No Universal Binary support
				# Should break down further to set the -mmacosx-version-min,
				# but the SDK detection is too unreliable here.
#			ENDIF(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
#		ENDIF(EXISTS /Developer/SDKs/MacOSX10.5.sdk)
	ENDIF(NOT ALMIXER_CONFIG_HAS_BEEN_RUN_BEFORE)
ENDIF(APPLE)

# This needs to be run very last so other parts of the scripts can take
# advantage of this.
IF(NOT ALMIXER_CONFIG_HAS_BEEN_RUN_BEFORE)
	SET(ALMIXER_CONFIG_HAS_BEEN_RUN_BEFORE 1 CACHE INTERNAL "Flag to track whether this is the first time running CMake or if CMake has been configured before")
ENDIF(NOT ALMIXER_CONFIG_HAS_BEEN_RUN_BEFORE)



