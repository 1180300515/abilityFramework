#include "ability_message.h"

#include <glog/logging.h>

namespace AbilityMessage {

Json::Value AbilityStatus::toJson() const {
  Json::Value root;
  root["abilityName"] = abilityName;
  root["state"] = state;

  if (!location.scene.empty()) {
    root["location"]["x"] = location.x;
    root["location"]["y"] = location.y;
    root["location"]["z"] = location.z;
    root["location"]["scene"] = location.scene;
  }

  if (!depend.isNull()){
    root["depend"] = depend;
  }
  for(const auto& [name , value] : control) {
    root["control"][name] = value;
  }
  for(const auto& [name, value] : obs) {
    root["obs"][name] = value;
  }
  return root;
}

bool AbilityStatus::parseJson(const Json::Value &root) {
  abilityName = root["abilityName"].asString();
  // 解析location
  if (root.isMember("location") && root["location"].isObject()) {
    const Json::Value &locationObj = root["location"];
    location.x = locationObj["x"].asFloat();
    location.y = locationObj["y"].asFloat();
    location.z = locationObj["z"].asFloat();
    location.scene = locationObj["scene"].asString();
  }
  // 解析control
  if (root.isMember("control") && root["control"].isObject()) {
    const Json::Value &controlObj = root["control"];
    for (const auto &member : controlObj.getMemberNames()) {
      control[member] = controlObj[member].asString();
    }
  }
  // 解析obs
  if (root.isMember("obs") && root["obs"].isObject()) {
    const Json::Value &obsObj = root["obs"];
    for (const auto &member : obsObj.getMemberNames()) {
      obs[member] = obsObj[member].asString();
    }
  }
  // 解析depend
  if (root.isMember("depend")) {
    depend = root["depend"];
  }
  state = root["state"].asString();
  return true; // 解析成功
}

bool AbilityCommand::parseJson(const Json::Value &root) {
  abilityName = root["abilityName"].asString();
  type = root["type"].asString();
  ip = root["ip"].asString();
  desireState = root["desireState"].asString();
  if (root.isMember("stateParam")) {
    stateParam = root["stateParam"];
  }
  if (root.isMember("intent")) {
    for (auto &item : root["intent"]) {
      desireProp new_d;
      new_d.controlName = item["controlName"].asString();
      new_d.value = item["controlIntent"].asString();
      desire.push_back(new_d);
    }
  }
  return true;
}

Json::Value AbilityCommand::toJson() const {
  Json::Value root;
  // Serialize basic fields
  root["abilityName"] = abilityName;
  root["type"] = type;
  root["ip"] = ip;
  root["desireState"] = desireState;
  if (!stateParam.empty()) {
    root["stateParam"] = stateParam;
  }

  // Serialize "intent" array
  if (!desire.empty()) {
    Json::Value desireArray(Json::arrayValue);
    for (const auto &prop : desire) {
      Json::Value item;
      if (!prop.controlName.empty()) {
        item["controlName"] = prop.controlName;
      }
      if (!prop.value.empty()) {
        item["controlIntent"] = prop.value;
      }
      desireArray.append(item);
    }
    root["intent"] = desireArray;
  }
  return root;
}
} // namespace AbilityMessage
