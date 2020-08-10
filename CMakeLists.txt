cmake_minimum_required(VERSION 3.5.1)

project(overstacked)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)

# Setting NDEBUG variable
message(STATUS "Build type selected = ${CMAKE_BUILD_TYPE}")
if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-DNDEBUG)
endif()

#Enabling warnings as errors
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(overstacked ${SOURCES})