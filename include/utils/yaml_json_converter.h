#ifndef UTILS_YAML_JSON_CONVERTER_H_
#define UTILS_YAML_JSON_CONVERTER_H_

#include <json/json.h>
#include <yaml-cpp/yaml.h>

#include <string>

/**
 * universal convert yaml to json , but will miss value type
 */
bool YamlToJson(const YAML::Node &ynode, Json::Value *jnode,
                std::string name = "");

/**
 * custom convert yaml to json , keep the value type , need to modify by
 * yourself
 */
bool YamlToJsonForInstance(const YAML::Node &ynode, Json::Value *jnode,
                           std::string name = "");

/**
 * json schema
 */
bool SchemaValidation(const std::string &schemajsonstring,
                      const std::string &inputstring);

// std::string JsonToString(const Json::Value &jnode);

// void StringToJson(const std::string jsonformatstring, Json::Value &jnode);

std::string GetCrdSchemaPart(const Json::Value &jnode);

std::string GetValidatePart(const Json::Value &jnode);

std::string GetInstanceKey(const Json::Value &jnode);

std::string GetInstanceKind(const Json::Value &jnode);

std::string StripSlashPrefix(const std::string &str);

#endif  // UTILS_YAML_JSON_CONVERTER_H_
