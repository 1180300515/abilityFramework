#include "glog/logging.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"

#include "controller/json_yaml_convert.h"

bool YamlToJson(const YAML::Node &ynode, Json::Value &jnode)
{
    try
    {
        if (ynode.IsScalar())
        {
            Json::Value v(ynode.Scalar());
            jnode.swapPayload(v);
            return true;
        }
        if (ynode.IsSequence())
        {
            for (size_t i = 0; i < ynode.size(); ++i)
            {
                Json::Value v;
                if (YamlToJson(ynode[i], v))
                {
                    jnode.append(v);
                }
                else
                {
                    return false;
                }
            }
        }
        else if (ynode.IsMap())
        {
            for (auto it = ynode.begin();
                 it != ynode.end(); ++it)
            {
                Json::Value v;
                if (YamlToJson(it->second, v))
                {
                    jnode[it->first.Scalar()] = v;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    catch (...)
    {
        LOG(ERROR) << "convert error";
        return false;
    }
    return true;
}

bool JsonToYaml(const Json::Value &jnode, YAML::Node &ynode)
{
    try
    {
        if (jnode.isArray())
        {
            for (int i = 0; i < jnode.size(); ++i)
            {
                YAML::Node n;
                if (JsonToYaml(jnode[i], n))
                {
                    ynode.push_back(n);
                }
                else
                {
                    return false;
                }
            }
        }
        else if (jnode.isObject())
        {
            for (auto it = jnode.begin();
                 it != jnode.end();
                 ++it)
            {
                YAML::Node n;
                if (JsonToYaml(*it, n))
                {
                    ynode[it.name()] = n;
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            ynode = jnode.asString();
        }
    }
    catch (...)
    {
        LOG(ERROR) << "convert error";
        return false;
    }
    return true;
}

std::string JsonToString(const Json::Value &jnode)
{
    Json::FastWriter writer;
    return writer.write(jnode);
}

void StringToJson(const std::string jsonformatstring, Json::Value &jnode)
{
    Json::Reader reader;
    reader.parse(jsonformatstring, jnode);
}

std::string GetCrdKey(const Json::Value &jnode)
{
    return jnode["metadata"]["name"].asString();
}

std::string GetCrdGroup(const Json::Value &jnode)
{
    return jnode["spec"]["group"].asString();
}

std::string GetCrdKind(const Json::Value &jnode)
{
    return jnode["spec"]["names"]["kind"].asString();
}

std::string GetCrdSchema(const Json::Value &jnode)
{
    Json::Value tag = jnode["spec"]["versions"];
    if (tag.isArray())
    {
        for (int i = 0; i < tag.size(); i++)
        {
            if (tag[i]["storage"].asString() == "true")
            {
                tag = tag[i]["schema"]["openAPIV3Schema"];
                break;
            }
            if (i == tag.size() - 1)
            {
                LOG(ERROR) << "format error can't excute schema";
            }
        }
        return JsonToString(tag);
    }
    else
    {
        LOG(ERROR) << "json format is not correct";
        return "";
    }
}

std::string CrdSchemaRevise(const std::string &schemastring)
{
    Json::Value jnode;
    StringToJson(schemastring, jnode);
    Json::Value mem = jnode["properties"]["spec"]["properties"]["properties"]["properties"];
    for (Json::Value::iterator it = mem.begin(); it != mem.end(); ++it)
    {
        jnode["properties"]["spec"]["properties"]["properties"]["properties"][it.name().c_str()]["type"] = "string";
    }
    jnode["properties"]["status"]["properties"]["occupancy"]["type"] = "string";
    return JsonToString(jnode);
}

std::string GetInstanceKey(const Json::Value &jnode)
{
    return jnode["metadata"]["name"].asString();
}

std::string GetInstanceGroupAndVersion(const Json::Value &jnode)
{
    return jnode["apiVersion"].asString();
}

std::string GetInstanceKind(const Json::Value &jnode)
{
    return jnode["kind"].asString();
}

std::string GetInstanceValidatePart(const Json::Value &jnode)
{
    Json::Value tag = jnode;
    tag.removeMember("apiVersion");
    tag.removeMember("kind");
    tag.removeMember("metadata");
    return JsonToString(tag);
}

bool SchemaValidation(const std::string schemajsonstring, const std::string inputstring)
{
    rapidjson::Document sd;
    if (sd.Parse(schemajsonstring.c_str()).HasParseError())
    {
        LOG(INFO) << "jsonschema is not valid:" << schemajsonstring;
        return false;
    }
    rapidjson::SchemaDocument schema(sd);
    rapidjson::Document d;
    if (d.Parse(inputstring.c_str()).HasParseError())
    {
        LOG(INFO) << "input is not a valid json:" << inputstring;
        return false;
    }
    rapidjson::SchemaValidator validator(schema);
    if (!d.Accept(validator))
    {
        LOG(INFO) << "can't pass the validate, please check the instance file";
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        LOG(INFO) << "Invalid schema: " << sb.GetString();
        LOG(INFO) << "Invalid keyword: " << validator.GetInvalidSchemaKeyword();
        sb.Clear();
        validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
        LOG(INFO) << "Invalid document: " << sb.GetString();
        return false;
    }
    else
    {
        LOG(INFO) << "validate success";
        return true;
    }
}

std::string GetAbilityKey(const Json::Value &jnode)
{
    return jnode["abilityname"].asString();
}
std::string GetAbilityGroupAndVersion(const Json::Value &jnode)
{
    return jnode["apiVersion"].asString();
}
std::string GetAbilityKind(const Json::Value &jnode)
{
    return jnode["kind"].asString();
}
std::string GetAbilityValidatePart(const Json::Value &jnode)
{
    Json::Value tag = jnode;
    tag.removeMember("apiVersion");
    tag.removeMember("kind");
    return JsonToString(jnode);
}