#include <httplib.h>
#include <json/json.h>

#include "heartbeat_info.h"

#include "ability_message.h"
#include <glog/logging.h>

std::string abilityName = "display2";

// 生命状态
std::string state = "INIT";

// 控制属性
std::string power = "off";
// 依赖信息
Json::Value depend;

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
  // 启动子能力
  Json::Value depend_info = depend["depend"]["camera"];
  // DLOG(INFO) << "依赖能力信息: " << depend_info.toStyledString();
  AbilityMessage::AbilityCommand cmd_to_camera;
  cmd_to_camera.abilityName = depend_info["abilityName"].asString();
  cmd_to_camera.type = depend_info["type"].asString();
  cmd_to_camera.ip = depend_info["ip"].asString();
  cmd_to_camera.desireState = "RUNNING";

  // DLOG(INFO) << "子能力命令: " << cmd_to_camera.toJson().toStyledString();

  httplib::Client ccc(cmd_to_camera.ip, 8080);
  auto res =
      ccc.Put("/ability/abilityCommand",
              cmd_to_camera.toJson().toStyledString(), "application/json");

  if (!res) {
    std::string err_mes = "fail to connect";
    DLOG(ERROR) << err_mes;
  }
  if (res->status != 200) {
    std::string err_mes = std::format("fail : {},{}", res->status, res->body);
    DLOG(ERROR) << err_mes;
  }
  // 自身状态
  abilityPort = ability_svr.bind_to_any_port("0.0.0.0");
  DLOG(INFO) << "分配能力端口" << abilityPort;
  state = "RUNNING";
  ability_thread = std::thread([&]() { ability_svr.listen_after_bind(); });
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

void onTerminate(bool mode) {
  DLOG(INFO) << "进入terminate";
  ability_svr.stop();
  abilityPort = -1;
  if (ability_thread.joinable()) {
    ability_thread.join();
  }
  state = "TERMINATE";
  if (!mode) {
    DLOG(INFO) << "只终止自身模式";
  } else {
    DLOG(INFO) << "终止依赖模式";
    Json::Value depend_info = depend["depend"]["camera"];
    AbilityMessage::AbilityCommand cmd_to_camera;
    cmd_to_camera.abilityName = depend_info["abilityName"].asString();
    cmd_to_camera.type = depend_info["type"].asString();
    cmd_to_camera.ip = depend_info["ip"].asString();
    cmd_to_camera.desireState = "TERMINATE";
    httplib::Client ccc(cmd_to_camera.ip, 8080);
    auto res =
        ccc.Put("/ability/abilityCommand",
                cmd_to_camera.toJson().toStyledString(), "application/json");

    if (!res) {
      std::string err_mes = "fail to connect";
      DLOG(ERROR) << err_mes;
    }
    if (res->status != 200) {
      std::string err_mes = std::format("fail : {},{}", res->status, res->body);
      DLOG(ERROR) << err_mes;
    }
  }
  heartbeatReport();
}

void handleCommand(const AbilityMessage::AbilityCommand& cmd) {
  if (cmd.desireState == "STANDBY" && state == "INIT") {
    onStart();
  } else if (cmd.desireState == "RUNNING") {
    depend = cmd.stateParam;
    // DLOG(INFO) << "配置depend: " << depend.toStyledString();
    onConnect();
  } else if (cmd.desireState == "STANDBY" && state == "RUNNING") {
    onDisconnect();
  } else if (cmd.desireState == "TERMINATE") {
    bool mode = true;
    if (cmd.stateParam.isMember("mode")) {
      mode = cmd.stateParam["mode"].asBool();
    }
    onTerminate(mode);
  }
}

bool sendDesireToChild(const AbilityMessage::AbilityCommand& cmd_to_camera) {
  // DLOG(INFO) << "发送给子能力: " << cmd_to_camera.toJson().toStyledString();
  httplib::Client ccc(cmd_to_camera.ip, 8080);
  std::string url =
      "/ability/abilityPort?abilityName=" + cmd_to_camera.abilityName;
  auto res = ccc.Get(url);
  if (!res) {
    std::string err_mes = "fail to connect";
    DLOG(ERROR) << err_mes;
    return false;
  }
  if (res->status != 200) {
    std::string err_mes = std::format("fail : {},{}", res->status, res->body);
    DLOG(ERROR) << err_mes;
    return false;
  }
  // DLOG(INFO) << "接收到端口回复: " << res->body;
  Json::Reader reader;
  Json::Value root;
  bool success = reader.parse(res->body, root);
  if (!success) {
    DLOG(ERROR) << "json parse fail";
    return false;
  }
  int cameraPort = root["abilityPort"].asInt();
  DLOG(INFO) << "子能力端口: " << cameraPort;
  if (cameraPort == -1) {
    DLOG(ERROR) << "能力: " << cmd_to_camera.abilityName << " 未运行";
    return false;
  } else {
    httplib::Client to_ability(cmd_to_camera.ip, cameraPort);
    res = to_ability.Put("/setDesire", cmd_to_camera.toJson().toStyledString(),
                         "application/json");
    if (!res) {
      std::string err_mes = "fail to connect";
      DLOG(ERROR) << err_mes;
      return false;
    }
    if (res->status != 200) {
      std::string err_mes = std::format("fail : {},{}", res->status, res->body);
      DLOG(ERROR) << err_mes;
      return false;
    }
  }
  return true;
}

void handleDesire(const AbilityMessage::AbilityCommand& cmd) {
  for (const auto& item : cmd.desire) {
    if (item.controlName == "power" && item.controlIntent == "on") {
      Json::Value depend_info = depend["depend"]["camera"];
      AbilityMessage::AbilityCommand cmd_to_camera;
      cmd_to_camera.abilityName = depend_info["abilityName"].asString();
      cmd_to_camera.type = depend_info["type"].asString();
      cmd_to_camera.ip = depend_info["ip"].asString();
      cmd_to_camera.desireState = "RUNNING";
      AbilityMessage::desireProp desire;
      desire.controlName = "power";
      desire.controlIntent = "on";
      cmd_to_camera.desire.emplace_back(desire);

      auto res = sendDesireToChild(cmd_to_camera);
      if (!res) {
        DLOG(ERROR) << "更新失败";
      } else {
        LOG(INFO) << "设置power为: " << item.controlIntent;
        power = item.controlIntent;
      }
    } else if (item.controlName == "power" && item.controlIntent == "off") {
      Json::Value depend_info = depend["depend"]["camera"];
      AbilityMessage::AbilityCommand cmd_to_camera;
      cmd_to_camera.abilityName = depend_info["abilityName"].asString();
      cmd_to_camera.type = depend_info["type"].asString();
      cmd_to_camera.ip = depend_info["ip"].asString();
      cmd_to_camera.desireState = "RUNNING";
      AbilityMessage::desireProp desire;
      desire.controlName = "power";
      desire.controlIntent = "off";
      cmd_to_camera.desire.emplace_back(desire);
      auto res = sendDesireToChild(cmd_to_camera);
      if (!res) {
        DLOG(ERROR) << "更新失败";
      } else {
        LOG(INFO) << "设置power为: " << item.controlIntent;
        power = item.controlIntent;
      }
    }
  }
}

std::string handleStatus() {
  AbilityMessage::AbilityStatus status;
  status.abilityName = abilityName;
  status.state = state;
  status.control["power"] = power;
  status.depend = depend;
  return status.toJson().toStyledString();
}

void Init() {
  ability_svr.Get("/getStatus",
                  [](const httplib::Request& req, httplib::Response& res) {
                    DLOG(INFO) << "能力: " << abilityName << "收到getStatus";
                    auto result = handleStatus();
                    res.status = 200;
                    res.set_header("Content-Type", "application/json");
                    res.set_content(result, "application/json");
                  });
  ability_svr.Put(
      "/setDesire", [](const httplib::Request& req, httplib::Response& res) {
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
        // DLOG(INFO) << "接收到 Desire " << cmd.toJson().toStyledString();
        handleDesire(cmd);
        res.status = 200;
      });

  lifeCycle_svr.Put(
      "/command", [](const httplib::Request& req, httplib::Response& res) {
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
        // DLOG(INFO) << "接收到 Command " << cmd.toJson().toStyledString();
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
  std::thread loop = std::thread([]() { report_loop(); });
  lifeCycle_svr.listen_after_bind();
}
