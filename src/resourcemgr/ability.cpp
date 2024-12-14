
#include "ability.h"

void Ability::parseJson(const Json::Value& root) {
  if (root.isMember("abilityName")) abilityName = root["abilityName"].asString();
  if (root.isMember("type")) type = root["type"].asString();

  // 解析locationDef结构体
  if (root.isMember("location") && root["location"].isObject()) {
    const Json::Value& locationObj = root["location"];
    location.x = locationObj["x"].asFloat();
    location.y = locationObj["y"].asFloat();
    location.z = locationObj["z"].asFloat();
    location.scene = locationObj["scene"].asString();
  }

  // 解析QoS信息
  if (root.isMember("QoS") && root["QoS"].isObject()) {
    const Json::Value& qosObj = root["QoS"];
    response = qosObj["response"].asFloat();
    reliability = qosObj["reliability"].asFloat();
    availability = qosObj["availability"].asFloat();
  }

  // 解析abilityDepend vector
  if (root.isMember("abilityDepend") && root["abilityDepend"].isArray()) {
    const Json::Value& abilityDependArray = root["abilityDepend"];
    for (const Json::Value& depend : abilityDependArray) {
      abilityDepend.push_back(depend.asString());
    }
  }

  if (root.isMember("frameworkIP")) frameworkIP = root["frameworkIP"].asString();
  if (root.isMember("state")) state = root["state"].asString();
}

Json::Value Ability::toJson() const {
  Json::Value root; // 将会包含Ability对象的数据
  root["abilityName"] = abilityName;
  root["type"] = type;

  // 将locationDef结构体转换为JSON对象
  Json::Value locationObj;
  locationObj["x"] = location.x;
  locationObj["y"] = location.y;
  locationObj["z"] = location.z;
  locationObj["scene"] = location.scene;
  root["location"] = locationObj;

  // 将QoS信息添加到JSON对象
  Json::Value qosObj;
  qosObj["response"] = response;
  qosObj["reliability"] = reliability;
  qosObj["availability"] = availability;
  root["QoS"] = qosObj;

  // 将abilityDepend vector转换为JSON数组
  if (abilityDepend.empty()) {
    Json::Value abilityDependArray;
    for (const auto& depend : abilityDepend) {
      abilityDependArray.append(depend);
    }
    root["abilityDepend"] = abilityDependArray;
  }

  root["frameworkIP"] = frameworkIP;
  root["state"] = state;

  return root;
}
