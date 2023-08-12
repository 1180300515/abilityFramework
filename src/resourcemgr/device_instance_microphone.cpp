#include "resourcemgr/device_instance_microphone.h"

#include <glog/logging.h>
#include <json/json.h>

#include "resourcemgr/hardware_audio.h"

std::string MicrophoneInstance::GetHardwareIdentifier() const {
  return spec.hardwareidentifier;
}

bool MicrophoneInstance::UpdateHardwareInfo(const Json::Value &info) {
  bool changeornot = false;
  AudioHardware hardware;
  hardware.fromJson(info);
  if (hardware.name == spec.hardwareidentifier) {
    if (spec.properties.hardwareName != hardware.name) {
      spec.properties.hardwareName = hardware.name;
      changeornot = true;
    }
    if (hardware.description != spec.properties.description) {
      spec.properties.description = hardware.description;
      changeornot = true;
    }
    if (spec.properties.sampleRate != hardware.sampleRate) {
      spec.properties.sampleRate = hardware.sampleRate;
      changeornot = true;
    }
    if (spec.properties.channels != hardware.channels) {
      spec.properties.channels = hardware.channels;
      changeornot = true;
    }
    if (spec.properties.format != hardware.format) {
      spec.properties.format = hardware.format;
      changeornot = true;
    }
    if (spec.properties.cardID != hardware.cardID) {
      spec.properties.cardID = hardware.cardID;
      changeornot = true;
    }
    if (spec.properties.deviceID != hardware.deviceID) {
      spec.properties.deviceID = hardware.deviceID;
      changeornot = true;
    }
  } else {
    DLOG(ERROR) << "this hardware info don't match this instance";
  }
  return changeornot;
}

void MicrophoneInstance::EraseHardwareInfo() {
  spec.properties.hardwareName = "";
  spec.properties.description = "";
  spec.properties.sampleRate.clear();
  spec.properties.channels.clear();
  spec.properties.format.clear();
  spec.properties.cardID = "";
  spec.properties.deviceID = "";
}

std::string MicrophoneInstance::Marshal() const {
  auto jnode = ToJson();
  Json::FastWriter writer;
  return writer.write(jnode);
}

Json::Value MicrophoneInstance::ToJson() const {
  Json::Value jnode;
  jnode["apiVersion"] = apiVersion;
  jnode["kind"] = kind;
  jnode["metadata"]["name"] = metadata.name;
  jnode["metadata"]["namespace"] = metadata.namespace_name;
  jnode["status"]["occupancy"] = status.occupancy;
  // spec part
  jnode["spec"]["kind"] = spec.kind;
  jnode["spec"]["version"] = spec.version;
  jnode["spec"]["hostname"] = spec.hostname;
  jnode["spec"]["hardwareidentifier"] = spec.hardwareidentifier;

  jnode["spec"]["properties"]["sampleRates"] = spec.properties.sampleRates;
  jnode["spec"]["properties"]["interface"] = spec.properties.interface;

  jnode["spec"]["properties"]["hardwareName"] = spec.properties.hardwareName;
  jnode["spec"]["properties"]["description"] = spec.properties.description;
  for (const auto &iter : spec.properties.sampleRate) {
    jnode["spec"]["properties"]["sampleRate"].append(iter);
  }
  for (const auto &iter : spec.properties.channels) {
    jnode["spec"]["properties"]["channels"].append(iter);
  }
  for (const auto &iter : spec.properties.format) {
    jnode["spec"]["properties"]["format"].append(iter);
  }
  jnode["spec"]["properties"]["cardID"] = spec.properties.cardID;
  jnode["spec"]["properties"]["deviceID"] = spec.properties.deviceID;

  for (int i = 0; i < spec.capability1.size(); i++) {
    Json::Value cap;
    cap["name"] = spec.capability1[i].name;
    for (int j = 0; j < spec.capability1[i].api.size(); j++) {
      cap["api"].append(spec.capability1[i].api[j]);
    }
    jnode["spec"]["capability1"].append(cap);
  }
  for (int i = 0; i < spec.capability2.size(); i++) {
    Json::Value cap;
    cap["name"] = spec.capability2[i].name;
    for (int j = 0; j < spec.capability2[i].api.size(); j++) {
      cap["api"].append(spec.capability2[i].api[j]);
    }
    jnode["spec"]["capability2"].append(cap);
  }
  // for (auto &iter : spec.customprops)
  // {
  //     jnode["spec"]["customprops"][iter.first] = iter.second;
  // }
  // api part
  for (int i = 0; i < api.function.size(); i++) {
    Json::Value cap;
    cap["name"] = api.function[i].name;
    for (int j = 0; j < api.function[i].param.size(); j++) {
      Json::Value param;
      param["name"] = api.function[i].param[j].name;
      param["type"] = api.function[i].param[j].type;
      param["index"] = api.function[i].param[j].index;
      cap["param"].append(param);
    }
    for (int k = 0; k < api.function[i].returnparam.size(); k++) {
      Json::Value returnparam;
      returnparam["name"] = api.function[i].returnparam[k].name;
      returnparam["type"] = api.function[i].returnparam[k].type;
      returnparam["index"] = api.function[i].returnparam[k].index;
      cap["returnparam"].append(returnparam);
    }
    jnode["api"]["function"].append(cap);
  }
  // devicelist part
  for (int i = 0; i < devicelist.size(); i++) {
    Json::Value cap;
    cap["devicename"] = devicelist[i].devicename;
    cap["deviceid"] = devicelist[i].deviceid;
    cap["deviceip"] = devicelist[i].deviceip;
    cap["deviceport"] = devicelist[i].deviceport;
    cap["status"] = devicelist[i].status;
    jnode["devicelist"].append(cap);
  }
  return jnode;
}

bool MicrophoneInstance::FromJson(const Json::Value &jnode) {
  DeviceInstanceInfo::FromJson(jnode);
  spec.kind = jnode["spec"]["kind"].asString();
  spec.hardwareidentifier = jnode["spec"]["hardwareidentifier"].asString();
  spec.version = jnode["spec"]["version"].asString();
  spec.hostname = jnode["spec"]["hostname"].asString();

  if (spec.capability1.size() != 0) {
    spec.capability1.clear();
  }
  if (spec.capability2.size() != 0) {
    spec.capability2.clear();
  }
  // if (spec.customprops.size() != 0)
  // {
  //     spec.customprops.clear();
  // }
  if (spec.properties.sampleRate.size() != 0) {
    spec.properties.sampleRate.clear();
  }
  if (spec.properties.channels.size() != 0) {
    spec.properties.channels.clear();
  }
  if (spec.properties.format.size() != 0) {
    spec.properties.format.clear();
  }

  if (jnode["spec"].isMember("capability1")) {
    for (int i = 0; i < jnode["spec"]["capability1"].size(); i++) {
      Acapability cap;
      cap.name = jnode["spec"]["capability1"][i]["name"].asString();
      for (int j = 0; j < jnode["spec"]["capability1"][i]["api"].size(); j++) {
        cap.api.push_back(jnode["spec"]["capability1"][i]["api"][j].asString());
      }
      spec.capability1.emplace_back(cap);
    }
  }
  if (jnode["spec"].isMember("capability2")) {
    for (int i = 0; i < jnode["spec"]["capability2"].size(); i++) {
      Acapability cap;
      cap.name = jnode["spec"]["capability2"][i]["name"].asString();
      for (int j = 0; j < jnode["spec"]["capability2"][i]["api"].size(); j++) {
        cap.api.push_back(jnode["spec"]["capability2"][i]["api"][j].asString());
      }
      spec.capability2.emplace_back(cap);
    }
  }
  spec.properties.sampleRates =
      jnode["spec"]["properties"]["sampleRates"].asString();
  spec.properties.interface =
      jnode["spec"]["properties"]["interface"].asString();

  spec.properties.hardwareName =
      jnode["spec"]["properties"]["hardwareName"].asString();
  spec.properties.description =
      jnode["spec"]["properties"]["description"].asString();
  for (const auto &iter : jnode["spec"]["properties"]["sampleRate"]) {
    spec.properties.sampleRate.emplace_back(iter.asUInt());
  }
  for (const auto &iter : jnode["spec"]["properties"]["channels"]) {
    spec.properties.channels.emplace_back(iter.asUInt());
  }
  for (const auto &iter : jnode["spec"]["properties"]["format"]) {
    spec.properties.format.emplace_back(iter.asString());
  }
  spec.properties.cardID = jnode["spec"]["properties"]["cardID"].asString();
  spec.properties.deviceID = jnode["spec"]["properties"]["deviceID"].asString();

  // if (jnode["spec"].isMember("customprops"))
  // {
  //     Json::Value::Members member;
  //     member = jnode["spec"]["customprops"].getMemberNames();
  //     for (Json::Value::Members::iterator it = member.begin(); it !=
  //     member.end(); it++)
  //     {
  //         spec.customprops[*it] =
  //         jnode["spec"]["customprops"][*it].asString();
  //     }
  // }
  return true;
}

bool MicrophoneInstance::UnMarshal(const std::string &data) {
  Json::Value jnode;
  Json::Reader reader;
  reader.parse(data, jnode);
  FromJson(jnode);
  return true;
}

bool MicrophoneInstance::updateInstance(const Json::Value &jnode) {
  return FromJson(jnode);
}

std::string MicrophoneInstance::getInstanceVersion() const {
  return spec.version;
}
