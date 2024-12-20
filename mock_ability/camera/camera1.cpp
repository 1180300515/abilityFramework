#include <httplib.h>
#include <json/json.h>

#include "heartbeat_info.h"

#include "ability_message.h"
#include <glog/logging.h>

std::string abilityName = "camera1";

// 生命状态
std::string state = "INIT";

// 控制属性
std::string power = "off";
// 观测属性
std::string rtsp = "rtsp://localhost:8554/stream";

// 端口
int IPCPort = -1;
int abilityPort = -1;

httplib::Client ccc("127.0.0.1", 8080);

httplib::Server lifeCycle_svr;
httplib::Server ability_svr;

std::thread ability_thread;

void heartbeatReport() {
    HeartbeatInfo info;
    info.abilityName = abilityName;
    info.IPCPort = IPCPort;
    info.abilityPort = abilityPort;
    info.state = state;
    auto res = ccc.Put("/ability/abilityHeartbeat",
                       info.toJson().toStyledString(), "application/json");
    if (!res) {
        std::string err_mes = "fail to connect";
        DLOG(ERROR) << err_mes;
    }
    if (res->status != 200) {
        std::string err_mes = std::format("fail : {},{}", res->status, res->body);
        DLOG(ERROR) << err_mes;
    }
}

void report_loop() {
    while (true) {
        if (state == "TERMINATE") {
            exit(0);
        }
        heartbeatReport();
        sleep(3);
    }
}

void onStart() {
    DLOG(INFO) << "进入standby";
    state = "STANDBY";
    heartbeatReport();
}

void onConnect() {
    DLOG(INFO) << "进入running";
    state = "RUNNING";
    abilityPort = ability_svr.bind_to_any_port("0.0.0.0");
    DLOG(INFO) << "分配能力端口" << abilityPort;
    ability_thread = std::thread([&]() {
        ability_svr.listen_after_bind();
    });
    heartbeatReport();
}

void onDisconnect() {
    DLOG(INFO) << "进入standby";
    ability_svr.stop();
    abilityPort = -1;
    if (ability_thread.joinable()) {
        ability_thread.join();
    }
    state = "STANDBY";
    heartbeatReport();
}

void onTerminate(bool mode = true) {
    DLOG(INFO) << "进入terminate";
    ability_svr.stop();
    abilityPort = -1;
    if (ability_thread.joinable()) {
        ability_thread.join();
    }
    state = "TERMINATE";
    heartbeatReport();
}

void handleCommand(const AbilityMessage::AbilityCommand &cmd) {
    if (cmd.desireState == "STANDBY" && state == "INIT") {
        onStart();
    } else if (cmd.desireState == "RUNNING") {
        onConnect();
    } else if (cmd.desireState == "STANDBY" && state == "RUNNING") {
        onDisconnect();
    } else if (cmd.desireState == "TERMINATE") {
        onTerminate();
    }
}

void handleDesire(const AbilityMessage::AbilityCommand &cmd) {
    for (const auto &item: cmd.desire) {
        if (item.controlName == "power") {
            DLOG(INFO) << "设置power为: " << item.controlIntent;
            power = item.controlIntent;
        }
    }
}

std::string handleStatus() {
    AbilityMessage::AbilityStatus status;
    status.abilityName = abilityName;
    status.state = state;
    status.control["power"] = power;
    status.obs["rtsp"] = rtsp;
    return status.toJson().toStyledString();
}

void Init() {
    ability_svr.Get("/getStatus",
                    [](const httplib::Request &req, httplib::Response &res) {
                        DLOG(INFO) << "能力: " << abilityName << "收到getStatus";
                        auto result = handleStatus();
                        res.status = 200;
                        res.set_header("Content-Type", "application/json");
                        res.set_content(result, "application/json");
                    });
    ability_svr.Put("/setDesire",
                    [](const httplib::Request &req, httplib::Response &res) {
                        DLOG(INFO) << "能力: " << abilityName << "收到setDesire";
                        AbilityMessage::AbilityCommand cmd;
                        Json::Reader reader;
                        Json::Value root;
                        bool success = reader.parse(req.body, root);
                        if (!success) {
                            DLOG(ERROR) << "json parse fail";
                            res.status = 400;
                            return;
                        }
                        cmd.parseJson(root);
                        handleDesire(cmd);
                    });

    lifeCycle_svr.Put("/command",
                      [](const httplib::Request &req, httplib::Response &res) {
                          DLOG(INFO) << "能力: " << abilityName << "收到command";
                          AbilityMessage::AbilityCommand cmd;
                          Json::Reader reader;
                          Json::Value root;
                          bool success = reader.parse(req.body, root);
                          if (!success) {
                              DLOG(ERROR) << "json parse fail";
                              res.status = 400;
                              return;
                          }
                          cmd.parseJson(root);
                          handleCommand(cmd);
                          res.status = 200;
                      });
}

int main() {
    google::InitGoogleLogging(abilityName.c_str());
    // 将日志输出到stderr（即控制台）
    FLAGS_alsologtostderr = true;

    Init();
    IPCPort = lifeCycle_svr.bind_to_any_port("127.0.0.1");

    std::thread loop = std::thread([]() {
        report_loop();
    });
    lifeCycle_svr.listen_after_bind();
}
