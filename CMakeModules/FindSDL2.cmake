# - Find SDL
# Find the SDL libraries (asound)
#
#  This module defines the following variables:
#     SDL_FOUND       - True if SDL_INCLUDE_DIR & SDL_LIBRARY are found
#     SDL_LIBRARY   - Set when SDL_LIBRARY is found
#
#     SDL_INCLUDE_DIR - where to find SDL.h, etc.
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

find_path(SDL_INCLUDE_DIR NAMES SDL.h 
	PATH_SUFFIXES include/SDL2 include
          DOC "The SDL include directory"
)

find_library(SDL_LIBRARY NAMES SDL2 sdl2 sdl2 sdl-2.0
          DOC "The SDL library"
)

if(WIN32)
	find_library(SDL_LIBRARY_MAIN NAMES SDL2main sdl2main 
		DOC "The SDLmain library needed on some platforms when builing an application (opposed to a library)"
)
else()
	set(SDL_LIBRARY_MAIN "")
endif()


# handle the QUIETLY and REQUIRED arguments and set SDL_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(SDL
                                  REQUIRED_VARS SDL_LIBRARY SDL_INCLUDE_DIR
                                  VERSION_VAR SDL_VERSION_STRING)

#mark_as_advanced(SDL_INCLUDE_DIR SDL_LIBRARIES)


