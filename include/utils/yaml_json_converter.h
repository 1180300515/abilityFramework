#ifndef _YAML_JSON_CONVERTER_H
#define _YAML_JSON_CONVERTER_H

#include "yaml-cpp/yaml.h"
#include "json/json.h"

/**
 * universal convert yaml to json , but will miss value type
 */
bool YamlToJson(const YAML::Node &ynode, Json::Value &jnode, std::string name = "");

/**
 * custom convert yaml to json , keep the value type , need to modify by yourself
 */
bool YamlToJsonForInstance(const YAML::Node &ynode, Json::Value &jnode, std::string name = "");

/**
 * json schema
 */
bool SchemaValidation(const std::string &schemajsonstring, const std::string &inputstring);

std::string JsonToString(const Json::Value &jnode);

void StringToJson(const std::string jsonformatstring, Json::Value &jnode);

std::string GetCrdSchemaPart(const Json::Value &jnode);

std::string GetAbilityValidatePart(const Json::Value &jnode);

std::string GetInstanceValidatePart(const Json::Value &jnode);

std::string GetInstanceKey(const Json::Value &jnode);

std::string GetInstanceKind(const Json::Value &jnode);

std::string StripSlashPrefix(const std::string &str);

#endif // _YAML_JSON_CONVERTER_H