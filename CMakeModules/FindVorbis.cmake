# - Find VORBIS
# Find the VORBIS libraries
#
#  This module defines the following variables:
#     VORBIS_FOUND       - True if VORBIS_INCLUDE_DIR & VORBIS_LIBRARY are found
#     VORBIS_LIBRARY   - Set when VORBIS_LIBRARY is found
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


# handle the QUIETLY and REQUIRED arguments and set VORBIS_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VORBIS
                                  REQUIRED_VARS VORBIS_LIBRARY VORBIS_INCLUDE_DIR
                                  VERSION_VAR VORBIS_VERSION_STRING)

#mark_as_advanced(VORBIS_INCLUDE_DIR VORBIS_LIBRARIES)


