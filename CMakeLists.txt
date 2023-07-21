cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang-14")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# set(YAML_INCLUDE_DIR  /usr/include/yaml-cpp)
# set(YAML_LIB_DIR /usr/lib/)

# set(GLOG_INCLUDE_DIR /usr/include/glog)
# set(GLOG_LIB_DIR /usr/lib/)

set(JSON_CPP_INCLUDE_DIR /usr/include/jsoncpp)
set(JSON_CPP_LIB_DIR /usr/lib/)

# set(RAPIDJSON_INCLUDE_DIR /usr/include/rapidjson)
# Macos specific
# set(YAML_DIR /opt/homebrew/Cellar/yaml-cpp/0.7.0)
# set(YAML_INCLUDE_DIR  ${YAML_DIR}/include/yaml-cpp)
# set(YAML_LIB_DIR ${YAML_DIR}/lib)

# Linux specific
# set(YAML_INCLUDE_DIR /usr/include/yaml-cpp)
# set(YAML_LIB_DIR /usr/lib/)

# set(PULSE_INCLUDE_DIR /usr/include/pulse)
# set(PULSE_LIB_DIR /usr/lib/pulseaudio)

# set(X11_INCLUDE_DIR /usr/include/X11)
# set(X11_LIB_DIR /usr/lib/)

# set(XRANDR_INCLUDE_DIR /usr/include/X11/extensions)
# set(XRANDR_LIB_DIR /usr/lib/)

# set(V4L2_INCLUDE_DIR /usr/include/linux)
# set(V4L2_LIB_DIR /usr/lib/)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/modules)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(PROTO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/protos)

#include_directories(${CMAKE_SOURCE_DIR}/include {YAML_INCLUDE_DIR} {GLOG_INCLUDE_DIR} {JSON_CPP_INCLUDE_DIR} {RAPIDJSON_INCLUDE_DIR}) 

find_package(yaml-cpp REQUIRED)
find_package(glog REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(X11 REQUIRED)
# find_package(protobuf REQUIRED)

find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# find_package(PkgConfig)
# pkg_search_module(GRPC REQUIRED grpc++)
# pkg_search_module(PROTOBUF REQUIRED protobuf)

# message(STATUS "=== PROTOBUF_LIBRARIES: ${PROTOBUF_LIBRARIES}")
# message(STATUS "=== PROTOBUF_INCLUDE_DIRS: ${PROTOBUF_INCLUDE_DIRS}")
# message(STATUS "=== GRPC_LIBRARIES: ${GRPC_LIBRARIES}")
# message(STATUS "=== GRPC_INCLUDE_DIRS: ${GRPC_INCLUDE_DIRS}")

# include_directories(${JSON_CPP_INCLUDE_DIR} ${PROTO_INCLUDE_DIR} ${PROTOBUF_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})

# add_library(Module_A SHARED ${CMAKE_SOURCE_DIR}/src/plugincore/pluginload.cpp)
# set_target_properties(Module_A PROPERTIES
#     CXX_STANDARD 17
#     CXX_STANDARD_REQUIRED ON
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/modules)
# target_include_directories(Module_A PUBLIC ${CMAKE_SOURCE_DIR}/include/plugincore)
# target_link_libraries(Module_A yaml-cpp)

# add_library(ability_mgr SHARED ${CMAKE_SOURCE_DIR}/src/abilitymanager/abimanager.cpp)
# set_target_properties(ability_mgr PROPERTIES
#     CXX_STANDARD 17
#     CXX_STANDARD_REQUIRED ON
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/modules)
# target_include_directories(ability_mgr PUBLIC ${CMAKE_SOURCE_DIR}/include/abilitymanager)
# target_link_libraries(ability_mgr)

# add_library(connection_mgr SHARED ${CMAKE_SOURCE_DIR}/src/connmanager/connmanager.cpp)
# set_target_properties(connection_mgr PROPERTIES
#     CXX_STANDARD 17
#     CXX_STANDARD_REQUIRED ON
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/modules)
# target_include_directories(connection_mgr PUBLIC ${CMAKE_SOURCE_DIR}/include/connmanager)
# target_link_libraries(connection_mgr)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/protos protos)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/abilityrelationmgr abilityrelationmgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/connectionmgr connectionmgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/databasemgr databasemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/discoverymgr discoverymgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/lifecyclemgr lifecyclemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/resourcemgr resourcemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/utils utils)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/httpserver httpserver)



include_directories(${PROJECT_SOURCE_DIR}/include/protos)
include_directories(${PROJECT_SOURCE_DIR}/include/utils)
include_directories(${PROJECT_SOURCE_DIR}/include/abilityrelationmgr)
include_directories(${PROJECT_SOURCE_DIR}/include/connectionmgr)
include_directories(${PROJECT_SOURCE_DIR}/include/databasemgr)
include_directories(${PROJECT_SOURCE_DIR}/include/discoverymgr)
include_directories(${PROJECT_SOURCE_DIR}/include/lifecyclemgr)
include_directories(${PROJECT_SOURCE_DIR}/include/resourcemgr)
include_directories(${PROJECT_SOURCE_DIR}/include/httpserver)


add_executable(${PROJECT_NAME} src/main.cpp 
    ${protos}
    ${abilityrelationmgr}
    ${connectionmgr}
    ${databasemgr}
    ${discoverymgr}
    ${lifecyclemgr}
    ${resourcemgr}
    ${utils}
    ${httpserver}
)
# add_executable(${PROJECT_NAME} src/main.cpp 
#     src/plugincore/pluginload.cpp 
#     src/plugincore/abimgr.cpp
#     src/eventhandler/eventhandler.cpp
#     src/discoverymanager/discoverymanager.cpp
#     src/discoverymanager/audiodevs.cpp
#     src/discoverymanager/camdevs.cpp
#     src/discoverymanager/dispdevs.cpp
#     src/discoverymanager/localhw_discovery.cpp
#     src/discoverymanager/discoverymanager.cpp
#     src/discoverymanager/devpool.cpp
#     src/protos/abilityProto.grpc.pb.cc
#     src/protos/abilityProto.pb.cc
#     ${SRC_LIST1} ${SRC_LIST2} ${SRC_LIST3})
# target_link_libraries(AbilityFramework sqlite3 JsonCpp::JsonCpp glog yaml-cpp X11 pulse Xrandr v4l2 pthread dl crypto ssl z protobuf ${GRPC_LIBRARIES} ${PROTOBUF_LIBRARIES} )
target_link_libraries(AbilityFramework stdc++ sqlite3 JsonCpp::JsonCpp glog yaml-cpp X11 pulse Xrandr v4l2 pthread dl crypto ssl z protobuf::libprotobuf gRPC::grpc++ )