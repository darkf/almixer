
# Required BLURRR_SDK_ROOT to be passed in or env
IF(NOT BLURRR_SDK_PATH)
	IF(NOT ENV{BLURRR_SDK_PATH}) 
		MESSAGE(AUTHOR_WARNING "BLURRR_SDK_PATH not passed in") 
	ELSE()
		SET(BLURRR_SDK_PATH $ENV{BLURRR_SDK_PATH})
	ENDIF()
ENDIF()
	
# Allow for the possibility that the user defined CMAKE_LIBRARY_PATH, etc. and the normal CMake stuff will work without the initial cache.
IF(BLURRR_SDK_PATH)
	# Convenience variables
	SET(BLURRR_FRAMEWORK_PATH "${BLURRR_SDK_PATH}/Frameworks")
	SET(BLURRR_INCLUDE_PATH "${BLURRR_SDK_PATH}/include")
	SET(BLURRR_LIBRARY_PATH "${BLURRR_SDK_PATH}/lib")

#	SET(CHIPMUNK_INCLUDE_DIR "${BLURRR_INCLUDE_PATH}/chipmunk" CACHE STRING "Chipmunk include directory")
#	SET(CHIPMUNK_LIBRARY "${BLURRR_LIBRARY_PATH}/libChipmunk.a" CACHE STRING "Chipmunk library")

	SET(LUA_INCLUDE_DIR "${BLURRR_INCLUDE_PATH}/lua" CACHE STRING "Lua include directory")
	SET(LUA_LIBRARY "${BLURRR_LIBRARY_PATH}/liblua.a" CACHE STRING "Lua library")	

ENDIF(BLURRR_SDK_PATH)
