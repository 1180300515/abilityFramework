/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: ability_instance_info.h
 * Description: ability_instance_info.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef RESOURCEMGR_ABILITY_INSTANCE_INFO_H_
#define RESOURCEMGR_ABILITY_INSTANCE_INFO_H_

#include <json/json.h>

#include <map>
#include <mutex>  // NOLINT [build/c++11]
#include <string>
#include <vector>

#include "resourcemgr/common_struct_defination.h"

// ability struct defination
struct Aabilityparam {
    std::string paramname;
    std::string paramtype;
    std::string index;
};
struct Aapi {
    std::string ApiType;
    std::string ApiName;
    std::vector<Aabilityparam> inputparam;
    std::vector<Aabilityparam> returnparam;
};
struct Aabilityinstance {
    std::string devicename;
    std::string deviceid;
    std::string deviceip;
    std::string port;
    std::string status;
};
struct Abilitydepends {
    std::vector<std::string> abilities;
    std::vector<std::string> devices;
};

/**
 * ability info defination
 */
class AbilityInstanceInfo {
   public:
    std::string apiVersion;
    std::string kind;
    Metadata metadata;
    std::string level;
    std::string abilityname;
    std::string description;
    std::string followed;
    std::string version;
    Abilitydepends depends;
    std::map<std::string, std::string> spec;
    std::vector<Aapi> ApiList;
    std::vector<Aabilityinstance> abilityinstancelist;

    bool UnMarshal(const std::string &data);
    bool FromJson(const Json::Value &jnode);
    std::string Marshal();
    bool updateAbility(const Json::Value &jnode);
};

#endif  // RESOURCEMGR_ABILITY_INSTANCE_INFO_H_
