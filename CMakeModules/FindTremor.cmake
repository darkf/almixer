# - Find TREMOR
# Find the TREMOR libraries
#
#  This module defines the following variables:
#     TREMOR_FOUND       - True if TREMOR_INCLUDE_DIR & TREMOR_LIBRARY are found
#     TREMOR_LIBRARY   - Set when TREMOR_LIBRARY is found
#
#     TREMOR_INCLUDE_DIR - where to find ivorbisfile.h, etc.
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

find_path(TREMOR_INCLUDE_DIR NAMES ivorbisfile.h 
	PATH_SUFFIXES include/tremor include
          DOC "The Tremor include directory"
)

find_library(TREMOR_LIBRARY NAMES tremor
          DOC "The Tremor library"
)


# handle the QUIETLY and REQUIRED arguments and set TREMOR_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TREMOR
                                  REQUIRED_VARS TREMOR_LIBRARY TREMOR_INCLUDE_DIR
                                  VERSION_VAR TREMOR_VERSION_STRING)

#mark_as_advanced(TREMOR_INCLUDE_DIR TREMOR_LIBRARIES)


