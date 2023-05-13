#include <httplib.h>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>


// 心跳信息
struct HeartbeatInfo {
    std::string status;
    std::chrono::steady_clock::time_point last_update;
};

// 心跳信息映射表
std::unordered_map<int, HeartbeatInfo> heartbeat_map;

// 互斥锁保护心跳信息映射表
std::mutex heartbeat_map_mutex;

// 打印心跳信息表的函数
void print_heartbeat_info() {
    // std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
    for (const auto& pair : heartbeat_map) {
        std::cout << "Port: " << pair.first << ", Status: " << pair.second.status
                  << ", Last update: " << std::chrono::duration_cast<std::chrono::seconds>(pair.second.last_update.time_since_epoch()).count() << " seconds ago\n";
    }
}

// 检查超时的函数
void check_timeout() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        auto now = std::chrono::steady_clock::now();
        for (auto it = heartbeat_map.begin(); it != heartbeat_map.end(); ) {
            if (now - it->second.last_update > std::chrono::seconds(20)) {
                it = heartbeat_map.erase(it);  // 超时，删除记录
                std::cout << "Erased one record" << std::endl;
            } else {
                ++it;
            }
        }
    }
}

void run_http_server() {
    auto svr = std::make_shared<httplib::Server>();

    svr->Post("/heartbeat", [](const httplib::Request& req, httplib::Response& res) {
        int port;
        try {
            port = std::stoi(req.get_param_value("port"));
        } catch (const std::exception&) {
            res.status = 400;  // Bad Request
            return;
        }

        std::lock_guard<std::mutex> lock(heartbeat_map_mutex);
        heartbeat_map[port] = {req.get_param_value("status"), std::chrono::steady_clock::now()};
        res.set_content("OK", "text/plain");

        print_heartbeat_info();
    });

    svr->listen("0.0.0.0", 8080);
}


bool start_program(const std::string& program_path) {
    pid_t pid = fork();

    if (pid < 0) {
        // 错误处理：fork失败
        return false;
    } else if (pid > 0) {
        // 我们在父进程中，pid是子进程的PID
    } else {
        // 我们在子进程中，启动新的程序
        execl(program_path.c_str(), program_path.c_str(), (char*) NULL);

        // 如果execl返回，那么说明出错了
        return false;
    }

    return true;
}
