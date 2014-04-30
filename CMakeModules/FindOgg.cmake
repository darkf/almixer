# - Find OGG
# Find the OGG libraries
#
#  This module defines the following variables:
#     OGG_FOUND       - True if OGG_INCLUDE_DIR & OGG_LIBRARY are found
#     OGG_LIBRARY   - Set when OGG_LIBRARY is found
#
#     OGG_INCLUDE_DIR - where to find OGG.h, etc.
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

find_path(OGG_INCLUDE_DIR NAMES ogg.h 
	PATH_SUFFIXES include/ogg include
          DOC "The OGG include directory"
)

find_library(OGG_LIBRARY NAMES ogg
          DOC "The OGG library"
)


# handle the QUIETLY and REQUIRED arguments and set OGG_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OGG
                                  REQUIRED_VARS OGG_LIBRARY OGG_INCLUDE_DIR
                                  VERSION_VAR OGG_VERSION_STRING)

#mark_as_advanced(OGG_INCLUDE_DIR OGG_LIBRARIES)


