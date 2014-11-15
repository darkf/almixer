# - Find alsa
# Find the alsa libraries (asound)
#
#  This module defines the following variables:
#     LUA_FOUND       - True if LUA_INCLUDE_DIR & LUA_LIBRARY are found
#     LUA_LIBRARY   - Set when LUA_LIBRARY is found
#
#     LUA_INCLUDE_DIR - where to find lua.h, etc.
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

find_path(LUA_INCLUDE_DIR NAMES lua.h
	PATH_SUFFIXES include/lua53 include/lua include lua53 lua
	PATHS
		$ENV{LUADIR}
		
	DOC "The Lua include directory"
)

find_library(LUA_LIBRARY NAMES lua53 lua
	PATHS
		$ENV{LUADIR}
		$ENV{LUADIR}/lib

	DOC "The Lua library"
)

# handle the QUIETLY and REQUIRED arguments and set LUA_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LUA
                                  REQUIRED_VARS LUA_LIBRARY LUA_INCLUDE_DIR
                                  VERSION_VAR LUA_VERSION_STRING)

mark_as_advanced(LUA_INCLUDE_DIR LUA_LIBRARY)


