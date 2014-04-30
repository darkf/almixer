# - Find MPG123
# Find the MPG123 libraries
#
#  This module defines the following variables:
#     MPG123_FOUND       - True if MPG123_INCLUDE_DIR & MPG123_LIBRARY are found
#     MPG123_LIBRARY   - Set when MPG123_LIBRARY is found
#
#     MPG123_INCLUDE_DIR - where to find MPG123.h, etc.
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

find_path(MPG123_INCLUDE_DIR NAMES mpg123.h 
	PATH_SUFFIXES include/MPG123 include
          DOC "The MPG123 include directory"
)

find_library(MPG123_LIBRARY NAMES mpg123
          DOC "The MPG123 library"
)


# handle the QUIETLY and REQUIRED arguments and set MPG123_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MPG123
                                  REQUIRED_VARS MPG123_LIBRARY MPG123_INCLUDE_DIR
                                  VERSION_VAR MPG123_VERSION_STRING)

#mark_as_advanced(MPG123_INCLUDE_DIR MPG123_LIBRARIES)


