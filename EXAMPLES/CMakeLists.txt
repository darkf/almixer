
#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OPENAL_INCLUDE_DIR} ${SDL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${OPENAL_INCLUDE_DIR})


#IF(APPLE)
#	IF(ALMIXER_COMPILE_WITH_SDL)
#		SET(SUPPORT_FILES SDLMain.m)
#	ENDIF(ALMIXER_COMPILE_WITH_SDL)
#ENDIF(APPLE)


ADD_EXECUTABLE(playstream ${CMAKE_CURRENT_SOURCE_DIR}/playstream.c ${SUPPORT_FILES})
#TARGET_LINK_LIBRARIES(playstream ALmixer ${OPENAL_LIBRARY} ${SDL_LIBRARY})
TARGET_LINK_LIBRARIES(playstream ALmixer ${OPENAL_LIBRARY})

ADD_EXECUTABLE(playsound ${CMAKE_CURRENT_SOURCE_DIR}/playsound.c ${SUPPORT_FILES})
#TARGET_LINK_LIBRARIES(playsound ALmixer ${OPENAL_LIBRARY} ${SDL_LIBRARY})
TARGET_LINK_LIBRARIES(playsound ALmixer ${OPENAL_LIBRARY})

# This is a little bit of a hack to get the example programs to play in place.
# For Apple @rpath, they must have a path to the location of the ALmixer library or framework.
# Typically, we don't build command line executables on Mac, but full .app bundles.
# ALmixer @rpath is aggressively configured for that case because it is the dominant use case.
# ALmixer is built with @rpath when made, not install (since drag and drop is the common case).
# This will fix it so the command line utils will find ALmixer in the above directory.
# This will probably break when you do a "make install".
# This could be fixed with bundling, or more paths, or turning off rpath.
# (Additional paths have been added to cover the common cases so hopefully this won't be a normal issue)
IF(APPLE)
	IF(WANTS_BUILD_SHARED_LIBRARY)
		SET_TARGET_PROPERTIES(playstream PROPERTIES INSTALL_RPATH "@loader_path;@loader_path/..;@loader_path/../bin")
		SET_TARGET_PROPERTIES(playsound PROPERTIES INSTALL_RPATH "@loader_path;@loader_path/..;@loader_path/../bin")
	ENDIF()
ENDIF()

