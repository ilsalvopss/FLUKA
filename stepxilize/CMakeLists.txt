cmake_minimum_required(VERSION 3.20)
project(gmsh)

set(CMAKE_BUILD_TYPE RelWithDebInfo)

set(CMAKE_CXX_STANDARD 14)

FIND_PACKAGE(OpenMP REQUIRED)

find_library(GMSH_LIB gmsh)
if(NOT GMSH_LIB)
    message(FATAL_ERROR "Could not find libgmsh")
endif()

find_path(GMSH_INC gmsh.h)
if(NOT GMSH_INC)
    message(FATAL_ERROR "Could not find gmsh.h")
endif()

include_directories(${GMSH_INC})
include_directories(/usr/local/include/opencascade)

add_executable(gmsh main.cpp)

target_link_libraries(gmsh ${GMSH_LIB} OpenMP::OpenMP_CXX)