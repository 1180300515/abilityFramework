#include "resourcemgr/message_package_struct.h"

#include <glog/logging.h>
#include <json/json.h>

bool UnMarshalMessageStruct(const std::string &data,
                            KeyAndDataPackages *kdpak) {
  Json::Reader reader;
  Json::Value jnode;
  bool err = reader.parse(data, jnode);
  if (!err) {
    DLOG(ERROR) << "json unmarshal fail";
    return false;
  }
  for (int i = 0; i < jnode["data"].size(); i++) {
    KeyDatapack pak;
    pak.key = jnode["data"][i]["key"].asString();
    if (jnode["data"][i].isMember("version")) {
      pak.version = jnode["data"][i]["version"].asString();
    }
    if (jnode["data"][i].isMember("eventType")) {
      pak.eventType = jnode["data"][i]["eventType"].asString();
    }
    if (jnode["data"][i].isMember("data")) {
      pak.data = jnode["data"][i]["data"].asString();
    }
    if (jnode["data"][i].isMember("resourceType")) {
      pak.resourceType = jnode["data"][i]["resourceType"].asString();
    }
    kdpak->data.emplace_back(pak);
  }
  kdpak->packageType = jnode["packageType"].asString();
  kdpak->hostname = jnode["hostname"].asString();
  return true;
}

std::string MarshalMessageStruct(const KeyAndDataPackages &kdpak) {
  Json::Value jnode;
  for (int i = 0; i < kdpak.data.size(); i++) {
    Json::Value key;
    key["key"] = kdpak.data[i].key;
    if (kdpak.data[i].version != "") {
      key["version"] = kdpak.data[i].version;
    }
    if (kdpak.data[i].eventType != "") {
      key["eventType"] = kdpak.data[i].eventType;
    }
    if (kdpak.data[i].resourceType != "") {
      key["resourceType"] = kdpak.data[i].resourceType;
    }
    if (kdpak.data[i].data != "") {
      key["data"] = kdpak.data[i].data;
    }

    jnode["data"].append(key);
  }
  jnode["packageType"] = kdpak.packageType;
  jnode["hostname"] = kdpak.hostname;
  Json::FastWriter writer;
  return writer.write(jnode);
}
