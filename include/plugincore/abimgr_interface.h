#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

#include <httplib.h>

#include "discoverymanager/localhw_interface.h"


// 心跳信息
struct HeartbeatInfo {
    std::string abilityName;
    std::string status;
    std::chrono::steady_clock::time_point last_update;
};

// 心跳信息映射表
extern std::unordered_map<int, HeartbeatInfo> heartbeat_map;

// 互斥锁保护心跳信息映射表
extern std::mutex heartbeat_map_mutex;

// 打印心跳信息表的函数
void print_heartbeat_info();

// 检查超时的函数
void check_timeout();

void run_http_server();

bool start_program(const std::string& program_path);




