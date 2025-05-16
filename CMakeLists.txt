cmake_minimum_required(VERSION 3.0)
project(rpc)

set(CMAKE_BUILD_TYPE "Debug")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)

link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了rpc框架所有的代码
add_subdirectory(src)
# example包含了使用rpc框架做事情的所有代码
add_subdirectory(example)
