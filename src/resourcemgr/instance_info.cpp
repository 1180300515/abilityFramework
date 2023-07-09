#include "instance_info.h"

#include "json/json.h"

bool InstanceInfo::UnMarshal(std::string source)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(source, jnode);
    apiVersion = jnode["apiVersion"].asString();
    kind = jnode["kind"].asString();
    metadata.name = jnode["metadata"]["name"].asString();

    if (api.function.size() != 0)
    {
        api.function.clear();
    }
    if (devicelist.size() != 0)
    {
        devicelist.clear();
    }

    if (jnode["metadata"].isMember("namespace"))
    {
        metadata.namespace_name = jnode["metadata"]["namespace"].asString();
    }
    else
    {
        metadata.namespace_name = "default";
    }
    status.occupancy = (jnode["status"]["occupancy"].asString() == "true");

    for (int i = 0; i < jnode["api"]["function"].size(); i++)
    {
        Afunction func;
        func.name = jnode["api"]["function"][i]["name"].asString();
        for (int j = 0; j < jnode["api"]["function"][i]["param"].size(); j++)
        {
            Adeviceparam aparam;
            aparam.name = jnode["api"]["function"][i]["param"][j]["name"].asString();
            aparam.type = jnode["api"]["function"][i]["param"][j]["type"].asString();
            aparam.index = jnode["api"]["function"][i]["param"][j]["index"].asString();
            func.param.push_back(aparam);
        }
        for (int x = 0; x < jnode["api"]["function"][i]["returnparam"].size(); x++)
        {
            Adeviceparam aparam;
            aparam.name = jnode["api"]["function"][i]["param"][x]["name"].asString();
            aparam.type = jnode["api"]["function"][i]["param"][x]["type"].asString();
            aparam.index = jnode["api"]["function"][i]["param"][x]["index"].asString();
            func.returnparam.push_back(aparam);
        }
        api.function.push_back(func);
    }

    Json::Value::Members dl = jnode["devicelist"].getMemberNames();
    for (auto iter = dl.begin(); iter != dl.end(); iter++)
    {
        DeviceDescribe de;
        de.devicename = jnode["devicelist"][*iter]["devicename"].asString();
        de.deviceid = jnode["devicelist"][*iter]["deviceid"].asString();
        de.deviceip = jnode["devicelist"][*iter]["deviceip"].asString();
        de.deviceport = jnode["devicelist"][*iter]["deviceport"].asString();
        de.status = jnode["devicelist"][*iter]["status"].asString();
        devicelist.emplace_back(de);
    }
    return true;
}