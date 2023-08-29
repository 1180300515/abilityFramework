/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: main.cpp
 * Description: main.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include <dirent.h>
#include <glog/logging.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <thread>  // NOLINT [build/c++11]

#include "abilityrelationmgr/ability_relation_manager.h"
#include "discoverymgr/discovery_manager.h"
#include "httpserver/http_server.h"
#include "lifecyclemgr/lifecycle_manager.h"
#include "resourcemgr/resource_manager.h"
#include "utils/color.h"
#include "utils/global_var.h"

int main(int argc, char **argv)
{
    std::cout << RED << "Load Config From File" << NONE << std::endl;
    std::string config_file_path = "config/start_config.yaml";
    YAML::Node config;
    try {
        config = YAML::LoadFile(config_file_path);
    } catch (YAML::ParserException e) {
        std::cout << "config yaml is malformed." << std::endl;
        exit(0);
    } catch (YAML::BadFile e) {
        std::cout << "file can't be load" << std::endl;
        exit(0);
    }
    bool loadcrd = config["LoadCrdFile"].as<bool>();
    bool loaddevice = config["LoadDeviceFile"].as<bool>();
    bool loadability = config["LoadAbilityFile"].as<bool>();
    bool cleandb = config["CleanDB"].as<bool>();
    bool logintofile = config["LogIntoFile"].as<bool>();

    if (logintofile) {
        FLAGS_log_dir = LOG_FILE_PATH;
    }
    FLAGS_alsologtostderr = 1;
    FLAGS_colorlogtostderr = 1;
    google::InitGoogleLogging("abiltyframework-cpp");

    auto resource_manager = std::make_shared<ResourceManager>();
    auto lifecycle_manager = std::make_shared<LifeCycleManager>();
    auto http_server = std::make_shared<HttpServer>();
    auto discovery_manager = std::make_shared<DiscoveryManager>();
    auto ability_relation_manager = std::make_shared<AbilityRelationManager>();

    resource_manager->Init(cleandb);
    lifecycle_manager->Init(std::bind(&ResourceManager::AbilityExistJudge, resource_manager, std::placeholders::_1));
    discovery_manager->Init(std::bind(&ResourceManager::EndAddressDiscoveryResult, resource_manager, std::placeholders::_1));
    ability_relation_manager->Init(std::bind(&ResourceManager::GetAbilityInfoExtractList, resource_manager),
                                   std::bind(&ResourceManager::GetHardWareResourceList, resource_manager, std::placeholders::_1));
    http_server->Init(resource_manager, lifecycle_manager, ability_relation_manager);

    std::vector<std::string> crd_file_names;
    std::vector<std::string> instance_file_names;
    std::vector<std::string> ability_file_names;
    DIR *dir;
    struct dirent *ent;
    if (loadcrd) {
        if ((dir = opendir(CRD_FILE_PATH)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }
                crd_file_names.emplace_back(ent->d_name);
                // DLOG(INFO) << ent->d_name;
            }
            closedir(dir);
            // add data into crd table
            for (int i = 0; i < crd_file_names.size(); i++) {
                std::string file_path = std::string(CRD_FILE_PATH) + "/" + crd_file_names[i];
                if (!resource_manager->RegistCrd(file_path, true)) {
                    DLOG(ERROR) << "register fail";
                    exit(0);
                }
            }
        } else {
            DLOG(ERROR) << "Could not open directory : " << CRD_FILE_PATH;
        }
    }
    if (loaddevice) {
        if ((dir = opendir(INSTANCE_FILE_PATH)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }
                instance_file_names.emplace_back(ent->d_name);
            }
            closedir(dir);
            for (int i = 0; i < instance_file_names.size(); i++) {
                std::string file_path = std::string(INSTANCE_FILE_PATH) + "/" + instance_file_names[i];
                if (!resource_manager->AddDeviceInstance(file_path, true)) {
                    DLOG(ERROR) << "add instance fail from file : " << file_path;
                }
            }
        } else {
            DLOG(ERROR) << "Could not open directory : " << INSTANCE_FILE_PATH;
        }
    }
    if (loadability) {
        if ((dir = opendir(ABILITY_FILE_PATH)) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                    continue;
                }
                ability_file_names.emplace_back(ent->d_name);
            }
            closedir(dir);
            for (int i = 0; i < ability_file_names.size(); i++) {
                std::string file_path = std::string(ABILITY_FILE_PATH) + "/" + ability_file_names[i];
                if (!resource_manager->AddAbilityInstance(file_path, true)) {
                    DLOG(ERROR) << "add ability fail from file : " << file_path;
                }
            }
        } else {
            DLOG(ERROR) << "Could not open directory : " << ABILITY_FILE_PATH;
        }
    }

    resource_manager->Run();
    discovery_manager->Run();
    lifecycle_manager->Run();
    http_server->Run();
    return 0;
}
