cmake_minimum_required(VERSION 3.0)
project(AbilityFramework VERSION 0.1)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-std=c++17 -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(regenerate_proto_file ON)
set(proto_file_folder ${PROJECT_SOURCE_DIR}/resource/protos)
set(proto_file_path ${PROJECT_SOURCE_DIR}/resource/protos/abilityProto.proto)

find_package(Threads REQUIRED)
option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOBUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()

get_filename_component(ability_proto "${PROJECT_SOURCE_DIR}/resource/protos/abilityProto.proto" ABSOLUTE)
get_filename_component(ability_proto_path "${ability_proto}" PATH)

set(ability_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/abilityProto.pb.cc")
set(ability_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/abilityProto.pb.h")
set(ability_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/abilityProto.grpc.pb.cc")
set(ability_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/abilityProto.grpc.pb.h")

add_custom_command(
    OUTPUT "${ability_proto_srcs}" "${ability_proto_hdrs}" "${ability_grpc_srcs}" "${ability_grpc_hdrs}"
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
    --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
    -I "${ability_proto_path}"
    --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
    "${ability_proto}"
    DEPENDS "${ability_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

find_package(yaml-cpp REQUIRED)
find_package(glog REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(X11 REQUIRED)

find_package(Threads REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/abilityrelationmgr abilityrelationmgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/connectionmgr connectionmgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/databasemgr databasemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/discoverymgr discoverymgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/lifecyclemgr lifecyclemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/resourcemgr resourcemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/utils utils)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/httpserver httpserver)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/controllermgr controllermgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/abilitystatusmgr abilitystatusmgr)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/test/base_function)

#生成abilityframework可执行文件
add_executable(${PROJECT_NAME} src/main.cpp 
    ${ability_proto_srcs}
    ${ability_grpc_srcs}
    ${abilityrelationmgr}
    ${connectionmgr}
    ${databasemgr}
    ${discoverymgr}
    ${lifecyclemgr}
    ${resourcemgr}
    ${controllermgr}
    ${abilitystatusmgr}
    ${utils}
    ${httpserver}
)
target_link_libraries(AbilityFramework stdc++ sqlite3 jsoncpp glog yaml-cpp::yaml-cpp X11 asound Xrandr v4l2 pthread dl crypto ssl z protobuf::libprotobuf gRPC::grpc++ )

#生成cameratest可执行文件
add_executable(cameratest test/test_ability_lifecycle.cpp ${ability_proto_srcs} ${ability_grpc_srcs})
target_link_libraries(cameratest stdc++ protobuf::libprotobuf gRPC::grpc++ glog)

#生成testCameraHardware可执行文件
add_executable(testCameraHardware 
    test/test_camera_hardware.cpp
    test/base_function/src/get_camera_hardware.cpp)
target_link_libraries(testCameraHardware stdc++ v4l2)

#生成testMicHardware可执行文件
add_executable(testMicHardware 
    test/test_mic_hardware.cpp
    test/base_function/src/get_audio_hardware.cpp)
target_link_libraries(testMicHardware stdc++ asound)

#生成testSpeakerHardware可执行文件
add_executable(testSpeakerHardware 
    test/test_speaker_hardware.cpp
    test/base_function/src/get_audio_hardware.cpp)
target_link_libraries(testSpeakerHardware stdc++ asound)

#生成testDisplayHardware可执行文件
add_executable(testDisplayHardware 
    test/test_display_hardware.cpp
    test/base_function/src/get_display_hardware.cpp)
target_link_libraries(testDisplayHardware stdc++ X11 Xrandr)

#生成testHttpcommand可执行文件
add_executable(testHttpCommand test/test_http_command.cpp)
target_link_libraries(testHttpCommand stdc++ jsoncpp)

#生成testAbilityDependTree可执行文件
add_executable(testAbilityDependTree 
    test/test_ability_depend_tree.cpp)
target_link_libraries(testAbilityDependTree stdc++ jsoncpp yaml-cpp::yaml-cpp)

#生成testAbilityDependTreeWithDevices可执行文件
add_executable(testAbilityDependTreeWithDevices 
    test/test_ability_depend_tree_with_devices.cpp
    test/base_function/src/get_camera_hardware.cpp
    test/base_function/src/get_audio_hardware.cpp
    test/base_function/src/get_display_hardware.cpp)
target_link_libraries(testAbilityDependTreeWithDevices stdc++ jsoncpp yaml-cpp::yaml-cpp X11 Xrandr asound v4l2)

add_executable(testHash
    test/test_ability_hash.cpp)

# add_executable(testResourceMgr test/resource_mgr_test/test_resourcemgr.cpp
#     ${databasemgr}
#     ${resourcemgr}
#     ${utils})
# target_link_libraries(testResourceMgr stdc++ sqlite3 jsoncpp glog yaml-cpp X11 asound Xrandr v4l2 pthread dl crypto ssl z protobuf::libprotobuf gRPC::grpc++)