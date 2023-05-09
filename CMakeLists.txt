cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++-14")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# set(YAML_INCLUDE_DIR  usr/include/yaml-cpp)
# set(YAML_LIB_DIR usr/lib/)

# set(GLOG_INCLUDE_DIR usr/include/glog)
# set(GLOG_LIB_DIR usr/lib/)

# set(JSON_CPP_INCLUDE_DIR usr/include/jsoncpp)
# set(JSON_CPP_LIB_DIR usr/lib/)

# set(RAPIDJSON_INCLUDE_DIR /usr/include/rapidjson)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#include_directories(${CMAKE_SOURCE_DIR}/include {YAML_INCLUDE_DIR} {GLOG_INCLUDE_DIR} {JSON_CPP_INCLUDE_DIR} {RAPIDJSON_INCLUDE_DIR}) 
include_directories(${CMAKE_SOURCE_DIR}/include)

# find_package(yaml-cpp REQUIRED)
# find_package(glog REQUIRED)
# find_package(jsoncpp REQUIRED)

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

aux_source_directory(${PROJECT_SOURCE_DIR}/src/controller SRC_LIST1)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/controller/common SRC_LIST2)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/controller/mongoose SRC_LIST3)

add_executable(${PROJECT_NAME} src/main.cpp 
    src/plugincore/pluginload.cpp 
    src/eventhandler/eventhandler.cpp
    src/discoverymanager/discoverymanager.cpp
    src/discoverymanager/audiodevs.cpp
    src/discoverymanager/camdevs.cpp
    src/discoverymanager/dispdevs.cpp
    src/discoverymanager/localhw_discovery.cpp
    ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})
target_link_libraries(AbilityFramework sqlite3 jsoncpp glog yaml-cpp pulse X11 Xrandr v4l2 )