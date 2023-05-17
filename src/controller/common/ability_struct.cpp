#include "json/json.h"

#include "controller/common/ability_struct.h"

bool UnMarshal(const std::string source, Ability &des)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(source, jnode);
    des.apiVersion = jnode["apiVersion"].asString();
    des.kind = jnode["kind"].asString();
    des.level = jnode["level"].asString();
    des.abilityname = jnode["abilityname"].asString();
    des.description = jnode["description"].asString();
    des.followed = jnode["followed"].asString();
    for (int i = 0; i < jnode["required"].size(); i++)
    {
        des.required.push_back(jnode["required"][i].asString());
    }
    Json::Value::Members member;
    member = jnode["spec"].getMemberNames();
    for (Json::Value::Members::iterator it = member.begin(); it != member.end(); it++)
    {
        des.spec[*it] = jnode["spec"][*it].asString();
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
            api.inputparam.push_back(param);
        }
        for (int x = 0; x < jnode["ApiList"][i]["inputparam"].size(); x++)
        {
            Aabilityparam param;
            param.paramname = jnode["ApiList"][i]["inputparam"][x]["paramname"].asString();
            param.paramtype = jnode["ApiList"][i]["inputparam"][x]["paramtype"].asString();
            param.index = jnode["ApiList"][i]["inputparam"][x]["index"].asString();
            api.returnparam.push_back(param);
        }
        des.ApiList.push_back(api);
    }
    return true;
}

std::string Marshal(Ability &source)
{
    Json::Value jnode;
    jnode["apiVersion"] = source.apiVersion;
    jnode["kind"] = source.kind;
    jnode["metadata"]["name"] = source.metadata.name;
    jnode["metadata"]["namespace"] = source.metadata.namespace_name;
    jnode["level"] = source.level;
    jnode["abilityname"] = source.abilityname;
    jnode["description"] = source.description;
    jnode["followed"] = source.followed;
    for (int i = 0; i < source.required.size(); i++)
    {
        jnode["required"].append(source.required[i]);
    }
    std::map<std::string, std::string>::iterator iter;
    iter = source.spec.begin();
    while (iter != source.spec.end())
    {
        jnode["spec"][iter->first] = iter->second;
        iter++;
    }
    // api part
    for (int i = 0; i < source.ApiList.size(); i++)
    {
        Json::Value cap;
        cap["ApiName"] = source.ApiList[i].ApiName;
        cap["ApiType"] = source.ApiList[i].ApiType;
        for (int j = 0; j < source.ApiList[i].inputparam.size(); j++)
        {
            Json::Value param;
            param["paramname"] = source.ApiList[i].inputparam[j].paramname;
            param["paramtype"] = source.ApiList[i].inputparam[j].paramtype;
            param["index"] = source.ApiList[i].inputparam[j].index;
            cap["inputparam"].append(param);
        }
        for (int k = 0; k < source.ApiList[i].returnparam.size(); k++)
        {
            Json::Value returnparam;
            returnparam["paramname"] = source.ApiList[i].returnparam[k].paramname;
            returnparam["paramtype"] = source.ApiList[i].returnparam[k].paramtype;
            returnparam["index"] = source.ApiList[i].returnparam[k].index;
            cap["returnparam"].append(returnparam);
        }
        jnode["ApiList"].append(cap);
    }
    // devicelist part
    for (int i = 0; i < source.abilityinstancelist.size(); i++)
    {
        Json::Value cap;
        cap["devicename"] = source.abilityinstancelist[i].devicename;
        cap["deviceid"] = source.abilityinstancelist[i].deviceid;
        cap["deviceip"] = source.abilityinstancelist[i].deviceip;
        cap["port"] = source.abilityinstancelist[i].port;
        cap["status"] = source.abilityinstancelist[i].status;
        jnode["abilityinstancelist"].append(cap);
    }
    Json::FastWriter writer;
    return writer.write(jnode);
}
