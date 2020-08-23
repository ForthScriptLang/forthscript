cmake_minimum_required(VERSION 3.9.4)

include(CheckIPOSupported)

project(overstacked)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build)
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

# Setting NDEBUG variable
message(STATUS "Build type selected = ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE MATCHES Release)
	add_definitions(-DNDEBUG)
endif()

# Enabling warnings as errors
if (MSVC)
    add_compile_options(/W1)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

include_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(overstacked ${SOURCES})

# Enable lto on release
if (CMAKE_BUILD_TYPE MATCHES Release)
    check_ipo_supported(RESULT supported OUTPUT error)
    if ( supported )
        message(STATUS "LTO builds are supported")
        set_property(TARGET overstacked PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()

set_target_properties(overstacked PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${RUNTIME_OUTPUT_DIRECTORY})
set_target_properties(overstacked PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${RUNTIME_OUTPUT_DIRECTORY})