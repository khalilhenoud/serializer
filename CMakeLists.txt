cmake_minimum_required(VERSION 3.22)

project(serializer_package)

if(ENGINE_BUILD)
add_subdirectory(serializer)
else()
add_subdirectory(serializer_test)
endif()