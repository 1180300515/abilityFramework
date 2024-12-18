
#include "ability_manager.h"

#include "ability_message.h"
#include <glog/logging.h>
#include <httplib.h>

#include <utility>

std::optional<std::string>
abilityManager::handleAbilityCommand(const Json::Value& root) {
  AbilityMessage::AbilityCommand cmd;
  cmd.parseJson(root);

  if (cmd.desireState == "RUNNING") {
    if (get_state(cmd.abilityName) == "RUNNING") {
      // 更新属性
      cmd.desireState = "";
      pushTask(cmd);
    } else {
      // 先更新状态
      AbilityMessage::AbilityCommand state_cmd;
      state_cmd = cmd;
      state_cmd.desire.clear();
      state_cmd.priority = 1;
      pushTask(state_cmd);
      // 再更新属性
      if (!cmd.desire.empty()) {
        cmd.desireState = "";
        cmd.stateParam.clear();
        pushTask(cmd);
      }
    }
  } else if (cmd.desireState == "STANDBY") {
    if (get_state(cmd.abilityName) == "RUNNING") {
      cmd.stateParam.clear();
      cmd.desire.clear();
      cmd.priority = 1;
      pushTask(cmd);
    } else {
      DLOG(ERROR) << "能力： " << cmd.abilityName << " 未运行";
    }
  } else if (cmd.desireState == "TERMINATE") {
    if (get_state(cmd.abilityName) == "RUNNING") {
      cmd.desire.clear();
      cmd.priority = 1;
      pushTask(cmd);
    } else {
      DLOG(ERROR) << "操作不合法";
    }
  } else {
    DLOG(ERROR) << "不合法预期状态";
  }
  return std::nullopt;
}

void abilityManager::pushTask(const AbilityMessage::AbilityCommand& cmd) {
  std::unique_lock<std::mutex> lock(lock_queue);
  // DLOG(INFO) << "推入任务: " << cmd.toJson().toStyledString();
  taskQueue.push(cmd);
  cv.notify_one();
}

void abilityManager::Run() {
  queueClean = std::thread([this]() {
    while (true) {
      AbilityMessage::AbilityCommand task;
      {
        std::unique_lock<std::mutex> lock(lock_queue);
        // 等待任务队列不为空
        cv.wait(lock, [this] { return !taskQueue.empty(); });
        // 从队列中取出任务执行
        task = taskQueue.top();
        taskQueue.pop();
      }
      // DLOG(INFO) << "当前任务: " << task.toJson().toStyledString();
      if (task.desireState.empty()) {
        // 属性控制指令
        if (get_state(task.abilityName) != "RUNNING") {
          DLOG(ERROR) << "能力: " << task.abilityName << " 未运行";
        } else {

          int ability_port = get_ability_port(task.abilityName);
          if (ability_port == -1) {
            DLOG(ERROR) << "能力端口未获取";
            continue;
          }
          httplib::Client ccc("127.0.0.1", ability_port);

          auto res = ccc.Put("/setDesire", task.toJson().toStyledString(),
                             "application/json");
          if (!res) {
            DLOG(ERROR) << std::format(
                "fail to put setDesire :{} fail to connect : ",
                task.abilityName);
          }
          if (res->status != 200) {
            DLOG(ERROR) << std::format("fail to put setDesire: {} : {},{}",
                                       task.abilityName, res->status,
                                       res->body);
          }
        }
      } else {
        // 状态控制指令
        set_state(task);
      }
    }
  });
}

void abilityManager::Init(
    std::function<std::string(const std::string&)> get_state_,
    std::function<int(const std::string&)> get_ability_port_,
    std::function<void(const AbilityMessage::AbilityCommand&)> set_state_,
    std::function<std::unordered_map<std::string, Ability>()> get_ability_list_,
    std::function<Ability::locationDef(const std::string&)> get_location_) {
  get_state = std::move(get_state_);
  get_ability_port = std::move(get_ability_port_);
  set_state = std::move(set_state_);
  get_ability_list = std::move(get_ability_list_);
  get_location = get_location_;
}

std::string abilityManager::handleAbilityList() {
  Json::Value result;
  auto list = get_ability_list();
  for (auto& ability : list) {
    auto state = get_state(ability.first);
    ability.second.state = state;
    result.append(ability.second.toJson());
  }
  return result.toStyledString();
}

std::string abilityManager::handleAbilityStatus(const std::string& name) {
  auto state = get_state(name);
  if (state != "RUNNING") {
    DLOG(INFO) << "能力: " << name << "未运行";
    return "";
  }
  auto port = get_ability_port(name);
  httplib::Client ccc("127.0.0.1", port);
  auto res = ccc.Get("/getStatus");
  if (!res) {
    DLOG(ERROR) << "fail to getStatus : fail to connect";
    return "";
  }
  if (res->status != 200) {
    DLOG(ERROR) << "fail to getStatus : " << res->status << res->body;
    return "";
  }
  // DLOG(INFO) << "拉取能力状态: " << res->body;
  AbilityMessage::AbilityStatus status;
  Json::Reader reader;
  Json::Value root;
  bool success = reader.parse(res->body, root);
  if (!success) {
    DLOG(ERROR) << "json parse fail";
    return "";
  }
  status.parseJson(root);
  auto loc = get_location(status.abilityName);
  status.location.x = loc.x;
  status.location.y = loc.y;
  status.location.z = loc.z;
  status.location.scene = loc.scene;
  // DLOG(INFO) << "最终能力状态: " << status.toJson().toStyledString();
  return status.toJson().toStyledString();
}
