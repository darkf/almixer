# - Find VORBIS
# Find the VORBIS libraries
#
#  This module defines the following variables:
#     VORBIS_FOUND       - True if VORBIS_INCLUDE_DIR & VORBIS_LIBRARY are found
#     VORBIS_LIBRARY   - Set when VORBIS_LIBRARY is found
#     VORBIS_FILE_LIBRARY   - Set when VORBIS_FILE_LIBRARY is found
#     VORBIS_ENC_LIBRARY   - Set when VORBIS_ENC_LIBRARY is found
#	(enc lib is disabled for now)
#	VORBIS_LIBRARIES - vorbisfile and vorbis (and potentially vorbisenc)
#
#     VORBIS_INCLUDE_DIR - where to find VORBIS.h, etc.
#

#=============================================================================
# Copyright Eric Wing
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_path(VORBIS_INCLUDE_DIR NAMES vorbisfile.h 
	PATH_SUFFIXES include/vorbis include
          DOC "The Vorbis include directory"
)

find_library(VORBIS_LIBRARY NAMES vorbis
          DOC "The Vorbis library"
)

# Hmmm. On Mac, if built as a .framework, then this is part of a monolithic library
# Weird, all caps matters for MATCHES or it's a syntax error
if(NOT (VORBIS_LIBRARY MATCHES "NOTFOUND") )
	if(NOT ${VORBIS_LIBRARY} MATCHES ".framework")
		find_library(VORBIS_FILE_LIBRARY NAMES vorbisfile
         		DOC "The Vorbis file library"
		)
	endif()
endif()

# ALmixer doesn't need the enc library
#find_library(VORBIS_ENC_LIBRARY NAMES vorbisenc
#          DOC "The Vorbis enc library"
#)


set(VORBIS_LIBRARIES ${VORBIS_FILE_LIBRARY} ${VORBIS_LIBRARY})
# handle the QUIETLY and REQUIRED arguments and set VORBIS_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VORBIS
                                  REQUIRED_VARS VORBIS_FILE_LIBRARY VORBIS_LIBRARY VORBIS_LIBRARIES VORBIS_INCLUDE_DIR
                                  VERSION_VAR VORBIS_VERSION_STRING)

#mark_as_advanced(VORBIS_INCLUDE_DIR VORBIS_LIBRARIES)


