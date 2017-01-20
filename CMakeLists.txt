cmake_minimum_required(VERSION 2.8)

project(cpu-simulator CXX)

# Version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

# Compiler
message("Compiler:\n\t${CMAKE_CXX_COMPILER}")

# Set default build type to Release
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wconversion -Wswitch-default -Wdisabled-optimization -Wformat -Winit-self -Woverloaded-virtual -Wfloat-equal")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast -Wno-pragmas -Wno-write-strings")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# Sources and headers
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_LIST_DIR} src/*.cpp generated/*.cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/generated)

# Echo compile flags
message("Build type:\n\t${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS}")
endif()

#add_subdirectory(tools)
add_subdirectory(test)

# Generate instruction file
add_custom_target(gen_instruction
    COMMAND python ${CMAKE_CURRENT_LIST_DIR}/tools/gen_instruction.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

add_executable(dec2bin tools/dec2bin.cpp)

# Build executable
add_executable(simulator ${SOURCES})
add_dependencies(simulator gen_instruction)
target_link_libraries(simulator ncurses)

# Clean
add_custom_target(cmake-clean
    COMMAND rm -rf `find ${CMAKE_BINARY_DIR} -name \"*[cC][mM]ake*\" -and -not -name \"CMakeLists.txt\"`
    COMMAND rm -rf `find ${CMAKE_BINARY_DIR} -name \"Makefile\"`)

add_custom_target(full-clean
    COMMAND make clean
    COMMAND make cmake-clean)
