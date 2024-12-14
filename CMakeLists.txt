cmake_minimum_required(VERSION 3.12)
project(AbilityFramework VERSION 0.1)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

# 指定C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

aux_source_directory(${PROJECT_SOURCE_DIR}/src/broadcastmgr broadcastmgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/lifecyclemgr lifecyclemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/resourcemgr resourcemgr)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/httpserver httpserver)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/abilitymgr abilitymgr)

include_directories(${PROJECT_SOURCE_DIR}/include/broadcastmgr)
include_directories(${PROJECT_SOURCE_DIR}/include/httpserver)
include_directories(${PROJECT_SOURCE_DIR}/include/resourcemgr)
include_directories(${PROJECT_SOURCE_DIR}/include/lifecyclemgr)
include_directories(${PROJECT_SOURCE_DIR}/include/abilitymgr)

# 生成abilityframework可执行文件
add_executable(${PROJECT_NAME} src/main.cpp
        ${broadcastmgr}
        ${lifecyclemgr}
        ${resourcemgr}
        ${httpserver}
        ${abilitymgr}
)
target_include_directories(${PROJECT_NAME} PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(AbilityFramework sqlite3 jsoncpp glog yaml-cpp)

add_executable(camera1 mock_ability/camera/camera.cpp
        src/lifecyclemgr/heartbeat_info.cpp
        src/abilitymgr/ability_message.cpp)
target_include_directories(camera1 PRIVATE ${JSONCPP_INCLUDE_DIRS})
target_link_libraries(camera1 glog jsoncpp)

add_executable(httpserver test/http_server.cpp)