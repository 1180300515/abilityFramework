#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <mutex>
#include <memory>
#include <map>

#include "instance_info.h"
#include "ability_info.h"
#include "nonlocal_resource_info.h"
#include "hardware_scan.h"
#include "resource_sync_manager.h"

class ResourceManager
{
    std::shared_ptr<HardwareScan> hardware_;
    std::shared_ptr<ResourceSyncManager> sync_;

    std::map<std::string, std::string> versionRecord;
    std::map<std::string, std::shared_ptr<InstanceInfo>> instances_;
    std::map<std::string, std::shared_ptr<AbilityInfo>> abilities_;
    std::map<std::string, std::shared_ptr<NonLocalResource>> nonlocal_instances_;

    std::mutex nonlocal_lock_;
    std::mutex abilities_lock_;
    std::mutex instances_lock_;

    ResourceManager() = default;
    ~ResourceManager() = default;

    std::string isLocalResource(std::string key);

    void LoadLocalResource();
    void Init();

    void Run();
    void RefreshKVRecord();
    std::string GetNonLocalFormat();
};

#endif // RESOURCE_MANAGER_H