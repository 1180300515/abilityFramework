cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++11 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(YAML_DIR /opt/homebrew/Cellar/yaml-cpp/0.7.0)
set(YAML_INCLUDE_DIR  ${YAML_DIR}/include/yaml-cpp)
set(YAML_LIB_DIR ${YAML_DIR}/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include {YAML_INCLUDE_DIR})
find_package(yaml-cpp REQUIRED)

add_library(Module_A STATIC ${CMAKE_SOURCE_DIR}/src/plugincore/pluginload.cpp)
target_include_directories(Module_A PUBLIC ${CMAKE_SOURCE_DIR}/include/plugincore)
target_link_libraries(Module_A yaml-cpp)

add_executable(${PROJECT_NAME} src/main.cpp src/plugincore/pluginload.cpp)
target_link_libraries(AbilityFramework yaml-cpp)