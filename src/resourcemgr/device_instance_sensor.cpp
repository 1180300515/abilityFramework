#include "device_instance_sensor.h"

#include "json/json.h"

std::string SensorInstance::Marshal() const
{
    auto jnode = ToJson();
    Json::FastWriter writer;
    return writer.write(jnode);
}

Json::Value SensorInstance::ToJson() const
{
    Json::Value jnode;
    jnode["apiVersion"] = apiVersion;
    jnode["kind"] = kind;
    jnode["metadata"]["name"] = metadata.name;
    jnode["metadata"]["namespace"] = metadata.namespace_name;
    jnode["status"]["occupancy"] = status.occupancy;
    // spec part
    jnode["spec"]["kind"] = spec.kind;
    jnode["spec"]["hardwareidentifier"] = spec.hardwareidentifier;
    jnode["spec"]["version"] = spec.version;
    jnode["spec"]["hostname"] = spec.hostname;
    jnode["spec"]["properties"]["vendor"] = spec.properties.vendor;
    jnode["spec"]["properties"]["location"] = spec.properties.location;
    jnode["spec"]["properties"]["interface"] = spec.properties.interface;

    for (int i = 0; i < spec.capability1.size(); i++)
    {
        Json::Value cap;
        cap["name"] = spec.capability1[i].name;
        for (int j = 0; j < spec.capability1[i].api.size(); j++)
        {
            cap["api"].append(spec.capability1[i].api[j]);
        }
        jnode["spec"]["capability1"].append(cap);
    }
    for (int i = 0; i < spec.capability2.size(); i++)
    {
        Json::Value cap;
        cap["name"] = spec.capability2[i].name;
        for (int j = 0; j < spec.capability2[i].api.size(); j++)
        {
            cap["api"].append(spec.capability2[i].api[j]);
        }
        jnode["spec"]["capability2"].append(cap);
    }
    // for (auto &iter : spec.customprops)
    // {
    //     jnode["spec"]["customprops"][iter.first] = iter.second;
    // }
    // api part
    for (int i = 0; i < api.function.size(); i++)
    {
        Json::Value cap;
        cap["name"] = api.function[i].name;
        for (int j = 0; j < api.function[i].param.size(); j++)
        {
            Json::Value param;
            param["name"] = api.function[i].param[j].name;
            param["type"] = api.function[i].param[j].type;
            param["index"] = api.function[i].param[j].index;
            cap["param"].append(param);
        }
        for (int k = 0; k < api.function[i].returnparam.size(); k++)
        {
            Json::Value returnparam;
            returnparam["name"] = api.function[i].returnparam[k].name;
            returnparam["type"] = api.function[i].returnparam[k].type;
            returnparam["index"] = api.function[i].returnparam[k].index;
            cap["returnparam"].append(returnparam);
        }
        jnode["api"]["function"].append(cap);
    }
    // devicelist part
    for (int i = 0; i < devicelist.size(); i++)
    {
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

std::string SensorInstance::GetHardwareIdentifier() const
{
    // no need yet
    return std::string();
}

bool SensorInstance::UpdateHardwareInfo(const Json::Value &info)
{
    // no need yet
    return false;
}

void SensorInstance::EraseHardwareInfo()
{
    //nothing
}

bool SensorInstance::FromJson(const Json::Value &jnode)
{
    DeviceInstanceInfo::FromJson(jnode);
    spec.kind = jnode["spec"]["kind"].asString();
    spec.hardwareidentifier = jnode["spec"]["hardwareidentifier"].asString();
    spec.version = jnode["spec"]["version"].asString();
    spec.hostname = jnode["spec"]["hostname"].asString();
    if (jnode["spec"].isMember("capability1"))
    {
        for (int i = 0; i < jnode["spec"]["capability1"].size(); i++)
        {
            Acapability cap;
            cap.name = jnode["spec"]["capability1"][i]["name"].asString();
            for (int j = 0; j < jnode["spec"]["capability1"][i]["api"].size(); j++)
            {
                cap.api.push_back(jnode["spec"]["capability1"][i]["api"][j].asString());
            }
            spec.capability1.emplace_back(cap);
        }
    }
    if (jnode["spec"].isMember("capability2"))
    {
        for (int i = 0; i < jnode["spec"]["capability2"].size(); i++)
        {
            Acapability cap;
            cap.name = jnode["spec"]["capability2"][i]["name"].asString();
            for (int j = 0; j < jnode["spec"]["capability2"][i]["api"].size(); j++)
            {
                cap.api.push_back(jnode["spec"]["capability2"][i]["api"][j].asString());
            }
            spec.capability2.emplace_back(cap);
        }
    }
    spec.properties.vendor = jnode["spec"]["properties"]["vendor"].asString();
    spec.properties.location = jnode["spec"]["properties"]["location"].asString();
    spec.properties.interface = jnode["spec"]["properties"]["interface"].asString();
    // if (jnode["spec"].isMember("customprops"))
    // {
    //     Json::Value::Members member;
    //     member = jnode["spec"]["customprops"].getMemberNames();
    //     for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++)
    //     {
    //         spec.customprops[*it] = jnode["spec"]["customprops"][*it].asString();
    //     }
    // }
    return true;
}

bool SensorInstance::UnMarshal(const std::string &data)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(data, jnode);
    FromJson(jnode);
    return true;
}

bool SensorInstance::updateInstance(const Json::Value &jnode)
{
    return FromJson(jnode);
}

std::string SensorInstance::getInstanceVersion() const
{
    return spec.version;
}