cmake_minimum_required(VERSION 2.8)

project(cpu-simulator CXX)

# Version
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1-alpha)

# Compiler
message("Compiler:\n\t${CMAKE_CXX_COMPILER}")

# Set default build type to Release
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compile flags
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wswitch-default -Wdisabled-optimization -Wformat -Winit-self -Woverloaded-virtual -Wfloat-equal")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-old-style-cast -Wno-pragmas")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# Sources and headers
file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR} *.cpp)
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

# Echo compile flags
message("Build type:\n\t${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    message("Compile flags:\n\t${CMAKE_CXX_FLAGS}")
endif()

# Generate Simulator::initInst()
add_custom_target(gen_initInst
    COMMAND python ${CMAKE_CURRENT_LIST_DIR}/gen_initInst.py > ${CMAKE_CURRENT_LIST_DIR}/init_inst.cpp)

# Build executable
add_executable(simulator ${SOURCES})
add_dependencies(simulator gen_initInst)

# Clean
add_custom_target(cmake-clean
    COMMAND rm -rf `find ${CMAKE_BINARY_DIR} -name \"*[cC][mM]ake*\" -and -not -name \"CMakeLists.txt\"`
    COMMAND rm -rf `find ${CMAKE_BINARY_DIR} -name \"Makefile\"`)

add_custom_target(full-clean
    COMMAND make clean
    COMMAND make cmake-clean)
