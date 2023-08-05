#include "yaml_json_converter.h"

#include "glog/logging.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"

bool YamlToJson(const YAML::Node &ynode, Json::Value &jnode, std::string name)
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
            for (auto it = ynode.begin(); it != ynode.end(); ++it)
            {
                Json::Value v;
                if (YamlToJson(it->second, v, it->first.Scalar()))
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

bool YamlToJsonForInstance(const YAML::Node &ynode, Json::Value &jnode, std::string name)
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
                if (YamlToJsonForInstance(ynode[i], v))
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
            for (auto it = ynode.begin(); it != ynode.end(); ++it)
            {
                for (auto it = ynode.begin(); it != ynode.end(); ++it)
                {
                    if (name == "properties" && it->first.Scalar() == "wideAngle")
                    {
                        jnode[it->first.Scalar()] = it->second.as<int>();
                        continue;
                    }
                    else if (name == "properties" && it->first.Scalar() == "telephoto")
                    {
                        jnode[it->first.Scalar()] = it->second.as<bool>();
                        continue;
                    }
                    else if (name == "properties" && it->first.Scalar() == "channelNumber")
                    {
                        jnode[it->first.Scalar()] = it->second.as<int>();
                        continue;
                    }
                    else if (name == "properties" && it->first.Scalar() == "bitWidth")
                    {
                        jnode[it->first.Scalar()] = it->second.as<int>();
                        continue;
                    }
                    else if (name == "status" && it->first.Scalar() == "occupancy")
                    {
                        jnode[it->first.Scalar()] = it->second.as<bool>();
                        continue;
                    }
                    else
                    {
                        Json::Value v;
                        if (YamlToJsonForInstance(it->second, v, it->first.Scalar()))
                        {
                            jnode[it->first.Scalar()] = v;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                return true;
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

bool SchemaValidation(const std::string &schemajsonstring, const std::string &inputstring)
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
        //LOG(INFO) << "can't pass the validate, please check the instance file";
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
        // LOG(INFO) << "validate success";
        return true;
    }
}

std::string GetCrdSchemaPart(const Json::Value &jnode)
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
        Json::FastWriter writer;
        return writer.write(tag);
    }
    else
    {
        LOG(ERROR) << "json format is not correct";
        return "";
    }
}

std::string GetInstanceKey(const Json::Value &jnode)
{
    return jnode["metadata"]["namespace"].asString() + "/" + jnode["metadata"]["name"].asString();
}

std::string GetInstanceKind(const Json::Value &jnode)
{
    return jnode["kind"].asString();
}

std::string GetValidatePart(const Json::Value &jnode)
{
    Json::Value tag = jnode;
    tag.removeMember("apiVersion");
    tag.removeMember("kind");
    tag.removeMember("metadata");
    Json::FastWriter writer;
    return writer.write(jnode);
}

std::string StripSlashPrefix(const std::string &str)
{
    size_t pos = str.rfind('/');
    if (pos != std::string::npos)
    {
        return str.substr(pos + 1);
    }
    return str;
}