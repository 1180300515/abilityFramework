#include <iostream>
#include <thread>
#include <dirent.h>

#include "glog/logging.h"
#include "yaml-cpp/yaml.h"

#include "resource_manager.h"
#include "lifecycle_manager.h"
#include "connection_manager.h"
#include "http_server.h"
#include "discovery_manager.h"
#include "ability_relation_manager.h"
#include "global_var.h"
#include "color.h"

int main(int argc, char **argv)
{
    std::cout << RED << "Load Config From File" << NONE << std::endl;
    std::string config_file_path = "config/start_config.yaml";
    YAML::Node config;
    try
    {
        config = YAML::LoadFile(config_file_path);
    }
    catch (YAML::ParserException e)
    {
        std::cout << "config yaml is malformed." << std::endl;
        exit(0);
    }
    catch (YAML::BadFile e)
    {
        std::cout << "file can't be load" << std::endl;
        exit(0);
    }
    bool loadcrd = config["loadCrdFile"].as<bool>();
    bool loaddevice = config["loadDeviceFile"].as<bool>();
    bool loadability = config["loadAbilityFile"].as<bool>();
    bool loadfromdb = config["loadFromDB"].as<bool>();
    bool logintofile = config["logIntoFile"].as<bool>();

    if (logintofile)
    {
        FLAGS_log_dir = LOG_FILE_PATH;
    }
    FLAGS_alsologtostderr = 1;
    google::InitGoogleLogging("abiltyframework-cpp");

    auto resource_manager = std::make_shared<ResourceManager>();
    auto lifecycle_manager = std::make_shared<LifeCycleManager>();
    auto connection_manager = std::make_shared<ConnectionManager>();
    auto http_server = std::make_shared<HttpServer>();
    auto discovery_manager = std::make_shared<DiscoveryManager>();
    auto ability_relation_manager = std::make_shared<AbilityRelationManager>();

    resource_manager->Init(connection_manager, loadfromdb);
    lifecycle_manager->Init(std::bind(&ResourceManager::AbilityExistJudge, resource_manager, std::placeholders::_1));
    discovery_manager->Init(std::bind(&ConnectionManager::OnEndAddressRecordChange, connection_manager, std::placeholders::_1),
                            std::bind(&ResourceManager::EndAddressDiscoveryResult, resource_manager, std::placeholders::_1));
    ability_relation_manager->Init(std::bind(&ResourceManager::GetAbilityInfoExtractList, resource_manager),
                                   std::bind(&ResourceManager::GetHardWareResourceList, resource_manager, std::placeholders::_1));
    http_server->Init(resource_manager, lifecycle_manager, ability_relation_manager, connection_manager);

    std::vector<std::string> crd_file_names;
    std::vector<std::string> instance_file_names;
    std::vector<std::string> ability_file_names;
    DIR *dir;
    struct dirent *ent;
    if (loadcrd)
    {
        if ((dir = opendir(CRD_FILE_PATH)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                {
                    continue;
                }
                crd_file_names.emplace_back(ent->d_name);
                LOG(INFO) << ent->d_name;
            }
            closedir(dir);
            // add data into crd table
            for (int i = 0; i < crd_file_names.size(); i++)
            {
                std::string file_path = std::string(CRD_FILE_PATH) + "/" + crd_file_names[i];
                if (!resource_manager->RegistCrd(file_path, true))
                {
                    LOG(ERROR) << "register fail";
                    exit(0);
                }
            }
        }
        else
        {
            LOG(ERROR) << "Could not open directory : " << CRD_FILE_PATH;
        }
    }
    if (loaddevice)
    {
        if ((dir = opendir(INSTANCE_FILE_PATH)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                {
                    continue;
                }
                instance_file_names.emplace_back(ent->d_name);
            }
            closedir(dir);
            for (int i = 0; i < instance_file_names.size(); i++)
            {
                std::string file_path = std::string(INSTANCE_FILE_PATH) + "/" + instance_file_names[i];
                if (!resource_manager->AddDeviceInstance(file_path, true))
                {
                    LOG(ERROR) << "add instance fail from file : " << file_path;
                }
            }
        }
        else
        {
            LOG(ERROR) << "Could not open directory : " << INSTANCE_FILE_PATH;
        }
    }
    if (loadability)
    {
        if ((dir = opendir(ABILITY_FILE_PATH)) != NULL)
        {
            while ((ent = readdir(dir)) != NULL)
            {
                if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                {
                    continue;
                }
                ability_file_names.emplace_back(ent->d_name);
            }
            closedir(dir);
            for (int i = 0; i < ability_file_names.size(); i++)
            {
                std::string file_path = std::string(ABILITY_FILE_PATH) + "/" + ability_file_names[i];
                if (!resource_manager->AddAbilityInstance(file_path, true))
                {
                    LOG(ERROR) << "add ability fail from file : " << file_path;
                }
            }
        }
        else
        {
            LOG(ERROR) << "Could not open directory : " << ABILITY_FILE_PATH;
        }
    }

    resource_manager->Run(false, false);
    discovery_manager->Run();
    lifecycle_manager->Run();
    http_server->Run();
    return 0;
}
