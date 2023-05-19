cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Macos specific
# set(YAML_DIR /opt/homebrew/Cellar/yaml-cpp/0.7.0)
# set(YAML_INCLUDE_DIR  ${YAML_DIR}/include/yaml-cpp)
# set(YAML_LIB_DIR ${YAML_DIR}/lib)

# Linux specific
set(YAML_INCLUDE_DIR /usr/include/yaml-cpp)
set(YAML_LIB_DIR /usr/lib/)

set(PULSE_INCLUDE_DIR /usr/include/pulse)
set(PULSE_LIB_DIR /usr/lib/pulseaudio)

set(X11_INCLUDE_DIR /usr/include/X11)
set(X11_LIB_DIR /usr/lib/)

set(XRANDR_INCLUDE_DIR /usr/include/X11/extensions)
set(XRANDR_LIB_DIR /usr/lib/)

set(V4L2_INCLUDE_DIR /usr/include/linux)
set(V4L2_LIB_DIR /usr/lib/)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include {YAML_INCLUDE_DIR})
find_package(yaml-cpp REQUIRED)

add_library(Module_A SHARED ${CMAKE_SOURCE_DIR}/src/plugincore/pluginload.cpp)
set_target_properties(Module_A PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
target_include_directories(Module_A PUBLIC ${CMAKE_SOURCE_DIR}/include/plugincore)
target_link_libraries(Module_A yaml-cpp)

add_library(ability_mgr SHARED ${CMAKE_SOURCE_DIR}/src/abilitymanager/abimanager.cpp)
set_target_properties(ability_mgr PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
target_include_directories(ability_mgr PUBLIC ${CMAKE_SOURCE_DIR}/include/abilitymanager)
target_link_libraries(ability_mgr)

add_library(connection_mgr SHARED ${CMAKE_SOURCE_DIR}/src/connmanager/connmanager.cpp)

set_target_properties(connection_mgr PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
target_include_directories(connection_mgr PUBLIC ${CMAKE_SOURCE_DIR}/include/connmanager)

target_link_libraries(connection_mgr)


add_executable(${PROJECT_NAME} src/main.cpp 
    src/plugincore/pluginload.cpp 
    src/eventhandler/eventhandler.cpp
    src/discoverymanager/discoverymanager.cpp
    src/discoverymanager/audiodevs.cpp
    src/discoverymanager/camdevs.cpp
    src/discoverymanager/dispdevs.cpp
    src/discoverymanager/localhw_discovery.cpp
    src/discoverymanager/discoverymanager.cpp)
target_link_libraries(AbilityFramework yaml-cpp pulse X11 Xrandr v4l2 pthread dl)
