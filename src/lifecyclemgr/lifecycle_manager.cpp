
#include "lifecycle_manager.h"

#include <glog/logging.h>
#include <httplib.h>

void lifeCycleManager::Init() {
  DLOG(INFO) << "init lifecycle manager";
}

void lifeCycleManager::Run() {

}

bool lifeCycleManager::start_process(const std::string &abilityName) {
  pid_t pid = fork();

  if (pid < 0) {
    return false;
  } else if (pid > 0) {
  } else {
    const std::string &program_path = abilityName;
    DLOG(INFO) << "start process : " << abilityName;
    execl(("bin/" + program_path).c_str(), program_path.c_str(),
          reinterpret_cast<char *>(NULL));
    perror("execl fail : ");
    exit(0);
  }
  return true;
}

std::string lifeCycleManager::getState(const std::string& abilityName) {
  std::shared_lock<std::shared_mutex> lock(heartbeat_map_lock);
  auto it = this->heartbeat_map.find(abilityName);
  if (it != this->heartbeat_map.end()) {
    return it->second.state;
  }
  return "";
}

bool lifeCycleManager::handleAbilityHeartbeat(const Json::Value &info) {
  HeartbeatInfo heartbeatInfo;
  heartbeatInfo.parseJson(info);

  std::unique_lock<std::shared_mutex> locker(heartbeat_map_lock);
  heartbeat_map[heartbeatInfo.abilityName] = heartbeatInfo;
  return true;
}

void lifeCycleManager::lifeCycleCommand(
    const AbilityMessage::AbilityCommand &cmd) {
  if (cmd.desireState == "RUNNING") {
    auto state = getState(cmd.abilityName);
    if (state.empty()) {
      start_process(cmd.abilityName);
      int port = getIPCPort(cmd.abilityName);
      httplib::Client ccc("127.0.0.1", port);

      // 状态变为standby
      auto start_cmd = cmd;
      start_cmd.desireState = "STANDBY";
      start_cmd.stateParam = "";
      auto res = ccc.Put("/command", start_cmd.toJson().toStyledString(),
                         "application/json");
      if (!res) {
        DLOG(ERROR) << "fail to put command : fail to connect";
        return;
      }
      if (res->status != 200) {
        DLOG(ERROR) << std::format("fail to put command : {},{}", res->status,
                                   res->body);
        return;
      }
      // 状态变为running
      res = ccc.Put("/command", cmd.toJson().toStyledString(),
                    "application/json");
      if (!res) {
        DLOG(ERROR) << "fail to put command : fail to connect";
        return;
      }
      if (res->status != 200) {
        DLOG(ERROR) << std::format("fail to put command : {},{}", res->status,
                                   res->body);
        return;
      }

    } else if (state == "STANDBY") {
      int port = getIPCPort(cmd.abilityName);
      httplib::Client ccc("127.0.0.1", port);
      auto res = ccc.Put("/command", cmd.toJson().toStyledString(),
                    "application/json");
      if (!res) {
        DLOG(ERROR) << "fail to put command : fail to connect";
        return;
      }
      if (res->status != 200) {
        DLOG(ERROR) << std::format("fail to put command : {},{}", res->status,
                                   res->body);
        return;
      }
    }
  } else if (cmd.desireState == "standby") {
    int port = getIPCPort(cmd.abilityName);
    httplib::Client ccc("127.0.0.1", port);
    auto res = ccc.Put("/command", cmd.toJson().toStyledString(),
                       "application/json");
    if (!res) {
      DLOG(ERROR) << "fail to put command : fail to connect";
      return;
    }
    if (res->status != 200) {
      DLOG(ERROR) << std::format("fail to put command : {},{}", res->status,
                                 res->body);
      return;
    }
  } else if (cmd.desireState == "terminate") {
    int port = getIPCPort(cmd.abilityName);
    httplib::Client ccc("127.0.0.1", port);
    auto res = ccc.Put("/command", cmd.toJson().toStyledString(),
                       "application/json");
    if (!res) {
      DLOG(ERROR) << "fail to put command : fail to connect";
      return;
    }
    if (res->status != 200) {
      DLOG(ERROR) << std::format("fail to put command : {},{}", res->status,
                                 res->body);
      return;
    }
  }
}

int lifeCycleManager::getAbilityPort(const std::string &abilityName) {
  std::shared_lock<std::shared_mutex> lock(this->heartbeat_map_lock);
  auto it = this->heartbeat_map.find(abilityName);
  if (it == this->heartbeat_map.end()) {
    return 0;
  }
  return it->second.abilityPort;
}
int lifeCycleManager::getIPCPort(const std::string &abilityName) {
  std::shared_lock<std::shared_mutex> lock(this->heartbeat_map_lock);
  auto it = this->heartbeat_map.find(abilityName);
  if (it == this->heartbeat_map.end()) {
    return 0;
  }
  return it->second.IPCPort;
}
