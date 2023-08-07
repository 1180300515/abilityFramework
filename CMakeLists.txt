cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
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

#生成abilityframework可执行文件
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
target_link_libraries(AbilityFramework stdc++ sqlite3 jsoncpp glog yaml-cpp X11 asound Xrandr v4l2 pthread dl crypto ssl z protobuf::libprotobuf gRPC::grpc++ )

#生成test可执行文件
add_executable(cameratest test/test_ability_lifecycle.cpp ${protos})
target_link_libraries(cameratest stdc++ protobuf::libprotobuf gRPC::grpc++ )

#生成testGetAbilityRunning可执行文件
add_executable(testGetAbilityRunning test/test_get_abilityrunning.cpp)
target_link_libraries(testGetAbilityRunning stdc++)

#生成testGetAbilitySupport可执行文件
add_executable(testGetAbilitySupport test/test_get_abilitysupport.cpp)
target_link_libraries(testGetAbilitySupport stdc++)

#生成testGetDevice可执行文件
add_executable(testGetDevice test/test_get_device.cpp)
target_link_libraries(testGetDevice stdc++)

#生成testPostAbilityRequest可执行文件
add_executable(testPostAbilityRequest test/test_post_abilityrequest.cpp)
target_link_libraries(testPostAbilityRequest stdc++ jsoncpp)

add_executable(testResourceMgr test/resource_mgr_test/test_resourcemgr.cpp
    ${databasemgr}
    ${resourcemgr}
    ${utils})
target_link_libraries(testResourceMgr stdc++ sqlite3 jsoncpp glog yaml-cpp X11 asound Xrandr v4l2 pthread dl crypto ssl z protobuf::libprotobuf gRPC::grpc++)