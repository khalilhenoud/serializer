cmake_minimum_required(VERSION 3.22)

# set the project name
project(serializer VERSION 1.0)

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

add_subdirectory(external/math)
add_subdirectory(external/library)

# add the executable
add_library(${PROJECT_NAME} SHARED
		./source/utils.c
		./source/serializer_bin.c
		./include/serializer/internal/module.h
      )
			
target_link_libraries(${PROJECT_NAME} 
      PUBLIC library
      PUBLIC math)

target_include_directories(${PROJECT_NAME} PUBLIC
      "${PROJECT_SOURCE_DIR}/include"
      )