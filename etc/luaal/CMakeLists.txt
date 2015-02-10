PROJECT(luaal C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)


INCLUDE(CMakeDependentOption)
SET(CPACK_SET_DESTDIR TRUE)
INCLUDE(CPack)

# Make sure CMake can find our non-standard modules
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(Lua53)
FIND_PACKAGE(OpenAL)

IF(LUA_INCLUDE_DIR AND EXISTS "${LUA_INCLUDE_DIR}/lua.h")
	file(STRINGS "${LUA_INCLUDE_DIR}/lua.h" LUA_VERSION_MAJOR_LINE REGEX "^#define[ \t]+LUA_VERSION_MAJOR[ \t]+\"[0-9]+\"$")
	file(STRINGS "${LUA_INCLUDE_DIR}/lua.h" LUA_VERSION_MINOR_LINE REGEX "^#define[ \t]+LUA_VERSION_MINOR[ \t]+\"[0-9]+\"$")
	string(REGEX REPLACE "^#define[ \t]+LUA_VERSION_MAJOR[ \t]+\"([0-9]+)\"$" "\\1" LUA_VERSION_MAJOR "${LUA_VERSION_MAJOR_LINE}")
	string(REGEX REPLACE "^#define[ \t]+LUA_VERSION_MINOR[ \t]+\"([0-9]+)\"$" "\\1" LUA_VERSION_MINOR "${LUA_VERSION_MINOR_LINE}")

	SET(LUAPACKAGE_CDIR "lib/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" CACHE STRING "Path for Lua packaged platform specific things.")
	SET(LUAPACKAGE_LDIR "share/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" CACHE STRING "Path for Lua packages platform independent things.")
ENDIF()

INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR} ${OPENAL_INCLUDE_DIR})


IF(APPLE AND IOS)
	OPTION(WANTS_BUILD_MODULE_LIBRARY "Set to ON to build loadable module." OFF)
	OPTION(WANTS_BUILD_STATIC_LIBRARY "Set to ON to build static library." ON)
ELSE()
	OPTION(WANTS_BUILD_MODULE_LIBRARY "Set to ON to build loadable module." ON)
	OPTION(WANTS_BUILD_STATIC_LIBRARY "Set to ON to build static library." OFF)
ENDIF()

IF(APPLE)
	SET(MODULE_LINK_FLAGS "-undefined dynamic_lookup")
ENDIF(APPLE)



SET(luaal_SOURCE_FILES
		${luaal_SOURCE_DIR}/luaal.c
)

SET(luaal_RESOURCE_FILES
)

IF(WIN32)
	LIST(APPEND luaal_RESOURCE_FILES ${luaal_SOURCE_DIR}/luaal.def)
ENDIF(WIN32)

IF(WANTS_BUILD_MODULE_LIBRARY)
	ADD_LIBRARY(luaal_library_module MODULE 
		${luaal_SOURCE_FILES} 
		${luaal_RESOURCE_FILES}
	)
	TARGET_LINK_LIBRARIES(luaal_library_module ${LUA_LIBRARY} ${OPENAL_LIBRARY} ${MODULE_LINK_FLAGS})

	IF(ANDROID)
		# The problem seems to be that Android won't externalize .so modules into /data/data/packagename/lib unless
		# the module starts with a "lib" prefix, e.g. libluaal.so instead of luaal.so. 
		# But simply renaming the file to libluaal.so causes internal assumptions to fail,
		# such as then needing to require('libluaal') instead of require('luaal').
		# But then if we do that, Lua tries to call luaopen_libluaal instead of luaopen_luaal.
		# So we can avoid this mess by prefixing lib to the name in the cpath.
		# REMEMBER: We must ship modules on Android with the lib prefix (libluaal.so).
		# But users don't need to know about this in their code.
		# Also note that Java System.loadLibrary implicitly assumes the lib prefix,
		# so this is important for that too.
		SET_TARGET_PROPERTIES(luaal_library_module PROPERTIES 
			# call the thing luaal
			OUTPUT_NAME "luaal"
		)
	ELSE()
		SET_TARGET_PROPERTIES(luaal_library_module PROPERTIES 
			# make sure "lib" doesn't become a prefix
			PREFIX ""
			# call the thing luaal
			OUTPUT_NAME "luaal"
		)
	ENDIF()

	IF(APPLE)
		INCLUDE(XcodeDefaults)
		BLURRR_CONFIGURE_XCODE_DEFAULTS(luaal_library_module)
	ENDIF()

	INSTALL(TARGETS
		luaal_library_module
		#	RUNTIME DESTINATION bin
		LIBRARY DESTINATION ${LUAPACKAGE_CDIR}
		ARCHIVE DESTINATION ${LUAPACKAGE_CDIR}
		#	ARCHIVE DESTINATION lib/static
	)

ENDIF(WANTS_BUILD_MODULE_LIBRARY)

IF(WANTS_BUILD_STATIC_LIBRARY)
	ADD_LIBRARY(luaal_library_static STATIC 
		${luaal_SOURCE_FILES} 
		${luaal_RESOURCE_FILES}
	)
#	TARGET_LINK_LIBRARIES(luaal_library_static ${LUA_LIBRARY})
	# FIXME: Might cause a name clash in Visual Studio
	SET_TARGET_PROPERTIES(luaal_library_static PROPERTIES OUTPUT_NAME "luaal")
	INSTALL(TARGETS
		luaal_library_static
		#	RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
		#	ARCHIVE DESTINATION lib/static
	)

	IF(APPLE)
		INCLUDE(XcodeDefaults)
		BLURRR_CONFIGURE_XCODE_DEFAULTS(luaal_library_static)
	ENDIF()


ENDIF(WANTS_BUILD_STATIC_LIBRARY)


INSTALL(FILES ${luaal_RESOURCE_FILES} DESTINATION ${LUAPACKAGE_LDIR})




# For uninstall (needs cmake_uninstall.cmake.in in the top-level directory)
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")



