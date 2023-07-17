#include "nonlocal_resource_info.h"

#include <memory>

#include "json/json.h"


bool UnMarshal(const std::string source, std::shared_ptr<NonLocalResource> des)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(source, jnode);
    des->key = jnode["key"].asString();
    des->hostname = jnode["hostname"].asString();
    des->kind = jnode["kind"].asString();
    for (int i = 0; i < jnode["devicelist"].size(); i++)
    {
        DeviceDescribe cap;
        cap.devicename = jnode["devicelist"][i]["devicename"].asString();
        cap.deviceid = jnode["devicelist"][i]["deviceid"].asString();
        cap.deviceport = jnode["devicelist"][i]["deviceport"].asString();
        cap.deviceip = jnode["devicelist"][i]["deviceip"].asString();
        cap.status = jnode["devicelist"][i]["status"].asString();
        des->devicelist.emplace_back(cap);
    }
    return true;
}

std::string Marshal(std::shared_ptr<NonLocalResource> source)
{
    Json::Value jnode;
    jnode["key"] = source->key;
    jnode["hostname"] = source->hostname;
    jnode["kind"] = source->kind;
    for (int i = 0; i < source->devicelist.size(); i++)
    {
        Json::Value cap;
        cap["devicename"] = source->devicelist[i].devicename;
        cap["deviceid"] = source->devicelist[i].deviceid;
        cap["deviceip"] = source->devicelist[i].deviceip;
        cap["deviceport"] = source->devicelist[i].deviceport;
        cap["status"] = source->devicelist[i].status;
        jnode["devicelist"].append(cap);
    }
    Json::FastWriter writer;
    return writer.write(jnode);
}