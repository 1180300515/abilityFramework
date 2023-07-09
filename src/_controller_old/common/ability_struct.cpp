#include "json/json.h"

#include "controller/common/ability_struct.h"

bool Ability::UnMarshal(const std::string source)
{
    std::lock_guard<std::mutex> locker(abilitylock_);
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(source, jnode);
    apiVersion = jnode["apiVersion"].asString();
    kind = jnode["kind"].asString();
    level = jnode["level"].asString();
    abilityname = jnode["abilityname"].asString();
    description = jnode["description"].asString();
    followed = jnode["followed"].asString();

    if (depends.abilities.size() != 0)
    {
        depends.abilities.clear();
    }
    if (depends.devices.size() != 0)
    {
        depends.devices.clear();
    }
    if (spec.size() != 0)
    {
        spec.clear();
    }
    if (ApiList.size() != 0)
    {
        ApiList.clear();
    }
    if (abilityinstancelist.size() != 0)
    {
        abilityinstancelist.clear();
    }

    if (jnode.isMember("depends"))
    {
        if (jnode["depends"].isMember("abilities"))
        {
            for (int i = 0; i < jnode["depends"]["abilities"].size(); i++)
            {
                depends.abilities.emplace_back(jnode["depends"]["abilities"][i].asString());
            }
        }
        if (jnode["depends"].isMember("devices"))
        {
            for (int i = 0; i < jnode["depends"]["devices"].size(); i++)
            {
                depends.devices.emplace_back(jnode["depends"]["devices"][i].asString());
            }
        }
    }
    Json::Value::Members member;
    member = jnode["spec"].getMemberNames();
    for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++)
    {
        spec[*it] = jnode["spec"][*it].asString();
    }

    for (int i = 0; i < jnode["ApiList"].size(); i++)
    {
        Aapi api;
        api.ApiType = jnode["ApiList"][i]["ApiType"].asString();
        api.ApiName = jnode["ApiList"][i]["ApiName"].asString();
        for (int j = 0; j < jnode["ApiList"][i]["inputparam"].size(); j++)
        {
            Aabilityparam param;
            param.paramname = jnode["ApiList"][i]["inputparam"][j]["paramname"].asString();
            param.paramtype = jnode["ApiList"][i]["inputparam"][j]["paramtype"].asString();
            param.index = jnode["ApiList"][i]["inputparam"][j]["index"].asString();
            api.inputparam.emplace_back(param);
        }
        for (int x = 0; x < jnode["ApiList"][i]["inputparam"].size(); x++)
        {
            Aabilityparam param;
            param.paramname = jnode["ApiList"][i]["inputparam"][x]["paramname"].asString();
            param.paramtype = jnode["ApiList"][i]["inputparam"][x]["paramtype"].asString();
            param.index = jnode["ApiList"][i]["inputparam"][x]["index"].asString();
            api.returnparam.emplace_back(param);
        }
        ApiList.emplace_back(api);
    }

    for (int i = 0; i < jnode["abilityinstancelist"].size(); i++)
    {
        Aabilityinstance instance;
        instance.devicename = jnode["abilityinstancelist"][i]["devicename"].asString();
        instance.deviceid = jnode["abilityinstancelist"][i]["deviceid"].asString();
        instance.deviceip = jnode["abilityinstancelist"][i]["deviceip"].asString();
        instance.port = jnode["abilityinstancelist"][i]["port"].asString();
        instance.status = jnode["abilityinstancelist"][i]["status"].asString();
        abilityinstancelist.emplace_back(instance);
    }
    return true;
}

std::string Ability::Marshal()
{
    std::lock_guard<std::mutex> locker(abilitylock_);
    Json::Value jnode;
    jnode["apiVersion"] = apiVersion;
    jnode["kind"] = kind;
    jnode["metadata"]["name"] = metadata.name;
    jnode["metadata"]["namespace"] = metadata.namespace_name;
    jnode["level"] = level;
    jnode["abilityname"] = abilityname;
    jnode["description"] = description;
    jnode["followed"] = followed;
    if (depends.abilities.size() != 0)
    {
        for (auto &iter : depends.abilities)
        {
            jnode["depends"]["abilities"].append(iter);
        }
    }
    if (depends.devices.size() != 0)
    {
        for (auto &iter : depends.devices)
        {
            jnode["depends"]["devices"].append(iter);
        }
    }
    for (auto &iter : spec)
    {
        jnode["spec"][iter.first] = iter.second;
    }
    // api part
    for (int i = 0; i < ApiList.size(); i++)
    {
        Json::Value cap;
        cap["ApiName"] = ApiList[i].ApiName;
        cap["ApiType"] = ApiList[i].ApiType;
        for (int j = 0; j < ApiList[i].inputparam.size(); j++)
        {
            Json::Value param;
            param["paramname"] = ApiList[i].inputparam[j].paramname;
            param["paramtype"] = ApiList[i].inputparam[j].paramtype;
            param["index"] = ApiList[i].inputparam[j].index;
            cap["inputparam"].append(param);
        }
        for (int k = 0; k < ApiList[i].returnparam.size(); k++)
        {
            Json::Value returnparam;
            returnparam["paramname"] = ApiList[i].returnparam[k].paramname;
            returnparam["paramtype"] = ApiList[i].returnparam[k].paramtype;
            returnparam["index"] = ApiList[i].returnparam[k].index;
            cap["returnparam"].append(returnparam);
        }
        jnode["ApiList"].append(cap);
    }
    // devicelist part
    for (int i = 0; i < abilityinstancelist.size(); i++)
    {
        Json::Value cap;
        cap["devicename"] = abilityinstancelist[i].devicename;
        cap["deviceid"] = abilityinstancelist[i].deviceid;
        cap["deviceip"] = abilityinstancelist[i].deviceip;
        cap["port"] = abilityinstancelist[i].port;
        cap["status"] = abilityinstancelist[i].status;
        jnode["abilityinstancelist"].append(cap);
    }
    Json::FastWriter writer;

    return writer.write(jnode);
}

bool Ability::updateAbility(std::string data)
{
    return UnMarshal(data);
}