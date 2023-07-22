cmake_minimum_required(VERSION 3.22)

# all libraries built by this project are copied into the output directory.
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(serializer_package)

# DOWNLOAD ALL THE SUBMODULES
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if(GIT_SUBMODULES)
		message(STATUS "Submodule update")
		execute_process(COMMAND $(GIT_EXECUTABLE) submodule upate --init --recursive 
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
		endif()
	endif()
endif()

# CHECK ALL THE SUBMODULES
#if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/glfw/CMakeLists.txt")
#	message(FATAL_ERROR "The glfw submdules was not downloaded!")
#endif()

add_subdirectory(external/math math)
add_subdirectory(external/library library)
add_subdirectory(serializer)
add_subdirectory(serializer_test)