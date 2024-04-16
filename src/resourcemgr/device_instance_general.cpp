#include "resourcemgr/device_instance_general.h"

#include <glog/logging.h>
#include <json/json.h>

#include <algorithm>

std::string GeneralDeviceInstance::GetHardwareIdentifier() const
{
    return "";
}
bool GeneralDeviceInstance::UpdateHardwareInfo(const Json::Value &info)
{
    this->info = info;
    return true;
}

void GeneralDeviceInstance::EraseHardwareInfo()
{
    info = Json::Value();
}

std::string GeneralDeviceInstance::Marshal() const
{
    auto root = ToJson();
    Json::FastWriter writer;
    return writer.write(root);
}

Json::Value GeneralDeviceInstance::ToJson() const
{
    return info;
}

bool GeneralDeviceInstance::FromJson(const Json::Value &jnode)
{
    DeviceInstanceInfo::FromJson(jnode);
    info = jnode;
    spec = jnode["spec"];
    spec["ip"] = jnode["ip"].asString();
    return true;
}

bool GeneralDeviceInstance::UnMarshal(const std::string &data)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(data, jnode);
    FromJson(jnode);
    return true;
}

bool GeneralDeviceInstance::updateInstance(const Json::Value &jnode)
{
    return FromJson(jnode);
}

std::string GeneralDeviceInstance::getInstanceVersion() const
{
    return spec.toStyledString();
}
