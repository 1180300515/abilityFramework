#include "resource_manager.h"

#include <fstream>

#include "json/json.h"
#include "yaml-cpp/yaml.h"

#include "database_manager.h"
#include "yaml_json_converter.h"
#include "global_var.h"
#include "device_instance_camera.h"
#include "device_instance_loudspeaker.h"
#include "device_instance_sensor.h"
#include "device_instance_microphone.h"
#include "message_package_struct.h"

ResourceManager::ResourceManager()
{
    DatabaseManager::getInstance();
}

bool ResourceManager::RegistCrd(const std::string &data, bool from_file)
{
    if (from_file)
    {
        return DatabaseManager::getInstance().RegistCrdFromFile(data);
    }
    else
    {
        return DatabaseManager::getInstance().RegistCrd(data);
    }
    return false;
}

bool ResourceManager::UnregistCrd(const std::string &name)
{
    return DatabaseManager::getInstance().UnregistCrd(name);
}

bool ResourceManager::AddAbilityInstance(const std::string &data, bool from_file)
{
    if (from_file)
    {
        YAML::Node instance_yaml;
        Json::Value instance_json;
        try
        {
            instance_yaml = YAML::LoadFile(data);
        }
        catch (YAML::ParserException e)
        {
            LOG(ERROR) << "yaml is malformed.";
            return false;
        }
        catch (YAML::BadFile e)
        {
            LOG(ERROR) << "file can't be load";
            return false;
        }
        if (!YamlToJson(instance_yaml, instance_json))
        {
            LOG(ERROR) << "convert the yaml file in: " << data << " to json format fail";
            return false;
        }
        if (DatabaseManager::getInstance().DBStoreAbilityInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            auto resource = std::make_shared<AbilityInstanceInfo>();
            resource->FromJson(instance_json);
            std::string key = GetInstanceKey(instance_json);
            abilities_[key] = resource;
            LOG(INFO) << "resource manager add ability instance : " << key;
            return true;
        }
        else
        {
            LOG(ERROR) << "resource manager add ability instance fail";
            return false;
        }
    }
    else
    {
        Json::Value instance_json;
        StringToJson(data, instance_json);
        if (DatabaseManager::getInstance().DBStoreAbilityInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            auto resource = std::make_shared<AbilityInstanceInfo>();
            resource->FromJson(instance_json);
            std::string key = GetInstanceKey(instance_json);
            abilities_[key] = resource;
            LOG(INFO) << "resource manager add ability instance : " << key;
            return true;
        }
        else
        {
            LOG(ERROR) << "resource manager add ability instance fail";
            return false;
        }
    }
    return false;
}

bool ResourceManager::UpdateAbilityInstance(const std::string &data, bool from_file)
{
    if (from_file)
    {
        // ignore
    }
    else
    {
        Json::Value instance_json;
        StringToJson(data, instance_json);
        if (DatabaseManager::getInstance().DBUpdateAbilityInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            std::string key = GetInstanceKey(instance_json);
            abilities_[key]->updateAbility(instance_json);
            LOG(INFO) << "resource manager update ability instance : " << key;
            return true;
        }
        else
        {
            LOG(ERROR) << "resource manager update ability instance fail";
            return false;
        }
    }
    return false;
}

bool ResourceManager::DeleteAbilityInstance(const std::string &key)
{
    if (abilities_.count(key) == 0)
    {
        LOG(INFO) << "the instance : " << key << " is not exist";
        return true;
    }
    else
    {
        if (DatabaseManager::getInstance().DBDeleteAbilityInstance(key))
        {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            abilities_.erase(key);
            LOG(INFO) << "resource manager delete ability instance : " << key;
            return true;
        }
        else
        {
            LOG(ERROR) << "resource manager delete ability instance fail";
        }
    }

    return false;
}

bool ResourceManager::AddDeviceInstance(const std::string &data, bool from_file)
{
    if (from_file)
    {
        YAML::Node instance_yaml;
        Json::Value instance_json;
        try
        {
            instance_yaml = YAML::LoadFile(data);
        }
        catch (YAML::ParserException e)
        {
            LOG(ERROR) << "yaml is malformed.";
            return false;
        }
        catch (YAML::BadFile e)
        {
            LOG(ERROR) << "file can't be load";
            return false;
        }
        if (!YamlToJsonForInstance(instance_yaml, instance_json))
        {
            LOG(ERROR) << "convert the yaml file in: " << data << " to json format fail";
            return false;
        }
        if (DatabaseManager::getInstance().DBStoreDeviceInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(devices_lock_);
            std::string device_kind = GetInstanceKind(instance_json);
            if (device_kind == CameraDeviceResourcetype)
            {
                auto new_device = std::make_shared<CameraInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == LoudspeakerDeviceResourcetype)
            {
                auto new_device = std::make_shared<LoudspeakerInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == MicrophoneDeviceResourcetype)
            {
                auto new_device = std::make_shared<MicrophoneInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == SensorDeviceResourcetype)
            {
                auto new_device = std::make_shared<SensorInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else
            {
                LOG(ERROR) << "unkonown resource type : " << device_kind;
                return false;
            }
        }
        else
        {
            LOG(ERROR) << "resource manager add device instance fail";
            return false;
        }
    }
    else
    {
        Json::Value instance_json;
        StringToJson(data, instance_json);
        if (DatabaseManager::getInstance().DBStoreDeviceInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(devices_lock_);
            std::string device_kind = GetInstanceKind(instance_json);
            if (device_kind == CameraDeviceResourcetype)
            {
                auto new_device = std::make_shared<CameraInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == LoudspeakerDeviceResourcetype)
            {
                auto new_device = std::make_shared<LoudspeakerInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == MicrophoneDeviceResourcetype)
            {
                auto new_device = std::make_shared<MicrophoneInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == SensorDeviceResourcetype)
            {
                auto new_device = std::make_shared<SensorInstance>();
                new_device->FromJson(instance_json);
                auto key = GetInstanceKey(instance_json);
                devices_[key] = new_device;
                LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
                return true;
            }
            else
            {
                LOG(ERROR) << "unkonown resource type : " << device_kind;
                return false;
            }
        }
        else
        {
            LOG(ERROR) << "resource manager add device instance fail";
            return false;
        }
    }
    return false;
}

bool ResourceManager::UpdateDeviceInstance(const std::string &data, bool from_file)
{
    if (from_file)
    {
        // ignore
    }
    else
    {
        Json::Value instance_json;
        StringToJson(data, instance_json);
        if (DatabaseManager::getInstance().DBUpdateDeviceInstance(instance_json))
        {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            std::string key = GetInstanceKey(instance_json);
            std::string device_kind = GetInstanceKind(instance_json);
            if (device_kind == CameraDeviceResourcetype)
            {
                auto new_device = std::dynamic_pointer_cast<CameraInstance>(devices_[key]);
                new_device->updateInstance(instance_json);
                LOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == LoudspeakerDeviceResourcetype)
            {
                auto new_device = std::dynamic_pointer_cast<LoudspeakerInstance>(devices_[key]);
                new_device->updateInstance(instance_json);
                LOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == MicrophoneDeviceResourcetype)
            {
                auto new_device = std::dynamic_pointer_cast<MicrophoneInstance>(devices_[key]);
                new_device->updateInstance(instance_json);
                LOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
                return true;
            }
            else if (device_kind == SensorDeviceResourcetype)
            {
                auto new_device = std::dynamic_pointer_cast<SensorInstance>(devices_[key]);
                new_device->updateInstance(instance_json);
                LOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
                return true;
            }
            else
            {
                LOG(ERROR) << "unkonown resource type : " << device_kind;
                return false;
            }
        }
        else
        {
            LOG(ERROR) << "resource manager update device instance fail";
            return false;
        }
    }
    return false;
}

bool ResourceManager::DeleteDeviceInstance(const std::string &key)
{
    if (devices_.count(key) == 0)
    {
        LOG(INFO) << "the instance : " << key << " is not exist";
        return true;
    }
    else
    {
        if (DatabaseManager::getInstance().DBDelteDeviceInstance(key))
        {
            std::lock_guard<std::mutex> locker(devices_lock_);
            devices_.erase(key);
            LOG(INFO) << "resource manager delete device instance : " << key;
            return true;
        }
        else
        {
            LOG(ERROR) << "resource manager delete device instance fail";
        }
    }
    return false;
}

std::string ResourceManager::isLocalResource(std::string key)
{
    std::lock_guard<std::mutex> locker1(this->devices_lock_);
    std::lock_guard<std::mutex> locker2(this->abilities_lock_);
    if (abilities_.count(key) != 0)
    {
        return AbilityResourcetype;
    }
    else if (devices_.count(key) != 0)
    {
        return devices_[key]->kind;
    }
    else
    {
        return "";
    }
    return std::string();
}

bool ResourceManager::AbilityExistJudge(const std::string &key)
{
    for (const auto &iter : abilities_)
    {
        if (key == iter.first)
        {
            return true;
        }
    }
    return false;
}

void ResourceManager::EndAddressDiscoveryResult(const std::map<std::string, std::string> &result)
{
    this->hardware_manager_->EndAddressResult(result);
}

void ResourceManager::LoadLocalResource()
{
    std::map<std::string,std::string> data;
    DatabaseManager::getInstance().DBGetAbilityInstances(data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            LOG(INFO) << "add resource : " << iter.first ;
            auto new_ability = std::make_shared<AbilityInstanceInfo>();
            new_ability->UnMarshal(iter.second);
            this->abilities_[iter.first] = new_ability;
        }
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(CameraDeviceResourcetype, data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            auto new_device = std::make_shared<CameraInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(MicrophoneDeviceResourcetype, data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            auto new_device = std::make_shared<MicrophoneInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(LoudspeakerDeviceResourcetype, data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            auto new_device = std::make_shared<LoudspeakerInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(SensorDeviceResourcetype, data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            auto new_device = std::make_shared<SensorInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    }
}

void ResourceManager::Init(std::shared_ptr<ConnectionManager> connect)
{
    getHostName();
    DatabaseManager::getInstance().Init(this->hostname_);
    LoadLocalResource();

    this->connection_ = connect;
    connection_->Init(std::bind(&ResourceManager::RecvMessageHandle, this, std::placeholders::_1));
    this->hardware_ = std::make_shared<HardwareScan>();
    std::shared_ptr<ResourceManager> p(this);
    this->hardware_->Init(p, this->hostname_);
    this->hardware_manager_ = std::make_shared<HardwareResourceManager>();
    this->hardware_manager_->Init(this->hostname_);
}

void ResourceManager::Run(bool startcloudsync, bool startendsync)
{
    if (startcloudsync)
    {
        this->cloudsyncThread_ = std::thread(&ResourceManager::cloudSyncThread, this);
    }
    if (startendsync)
    {
        this->endsyncThread_ = std::thread(&ResourceManager::endSyncThread, this);
    }
    this->hardware_->Run();
}

void ResourceManager::RefreshKVRecord()
{
    std::lock_guard<std::mutex> locker1(this->devices_lock_);
    std::lock_guard<std::mutex> locker2(this->abilities_lock_);
    this->versionRecord.clear();
    for (const auto &iter : devices_)
    {
        versionRecord[iter.first] = iter.second->getInstanceVersion();
    }
    for (const auto &iter : abilities_)
    {
        versionRecord[iter.first] = iter.second->version;
    }
}

std::string ResourceManager::GetHardwareDeviceInfo(bool format)
{
    return hardware_->GetHardwareDeviceInfo(format);
}

std::vector<AbilityInfoExtract> ResourceManager::GetAbilityInfoExtractList()
{
    std::vector<AbilityInfoExtract> result;
    std::lock_guard<std::mutex> locker(abilities_lock_);
    for (const auto &iter : abilities_)
    {
        AbilityInfoExtract info;
        info.name = StripSlashPrefix(iter.first);
        info.depends.abilities = iter.second->depends.abilities;
        info.depends.devices = iter.second->depends.devices;
        result.emplace_back(info);
    }
    return result;
}

void ResourceManager::addNonLocalResource(const std::string &data)
{
    std::lock_guard<std::mutex> locker(nonlocal_lock_);
    auto resource = std::make_shared<NonLocalResource>();
    UnMarshal(data, resource);
    nonlocal_instances_[resource->key] = resource;
}

void ResourceManager::cloudMessageHandle(const KeyAndDataPackages &data)
{
    KeyAndDataPackages thirdpack;

    if (data.packageType == CloudSyncStepTwo)
    {
        // record the non local hostname and key list
        std::map<std::string, std::list<std::string>> record;
        {
            std::lock_guard<std::mutex> locker(nonlocal_lock_);
            for (auto &iter : nonlocal_instances_)
            {
                record[iter.second->hostname].push_back(iter.first);
            }
        }
        for (int i = 0; i < data.data.size(); i++)
        {
            if (data.data[i].eventType == EdgeNonLocalEventType)
            {
                // add the non local resource
                auto resource = std::make_shared<NonLocalResource>();
                if (!UnMarshal(data.data[i].data, resource))
                {
                    LOG(ERROR) << "unmarshal fail";
                }
                this->nonlocal_instances_[data.data[i].key] = resource;
                if (record.find(resource->hostname) != record.end())
                {
                    record[resource->hostname].remove(data.data[i].key);
                }
            }
            else if (data.data[i].eventType == CloudAddEventType)
            {
                // cloudadd，云端资源需要添加或者更新
                std::string type = isLocalResource(data.data[i].key);
                if (type == "")
                {
                    LOG(ERROR) << "the resource: " << data.data[i].key << " is not the local resource";
                    continue;
                }
                KeyDatapack pack;
                std::string update_data;
                std::string pack_version;

                if (type == CameraDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    pack_version = std::dynamic_pointer_cast<CameraInstance>(devices_[data.data[i].key])->getInstanceVersion();
                    update_data = std::dynamic_pointer_cast<CameraInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == LoudspeakerDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    pack_version = std::dynamic_pointer_cast<LoudspeakerInstance>(devices_[data.data[i].key])->getInstanceVersion();
                    update_data = std::dynamic_pointer_cast<LoudspeakerInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == SensorDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    pack_version = std::dynamic_pointer_cast<SensorInstance>(devices_[data.data[i].key])->getInstanceVersion();
                    update_data = std::dynamic_pointer_cast<SensorInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == MicrophoneDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    pack_version = std::dynamic_pointer_cast<MicrophoneInstance>(devices_[data.data[i].key])->getInstanceVersion();
                    update_data = std::dynamic_pointer_cast<MicrophoneInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == AbilityResourcetype)
                {
                    std::lock_guard<std::mutex> lock(abilities_lock_);
                    pack_version = abilities_[data.data[i].key]->version;
                    update_data = abilities_[data.data[i].key]->Marshal();
                }
                pack.data = update_data;
                pack.eventType = data.data[i].eventType;
                pack.resourceType = type;
                pack.version = pack_version;
                pack.key = data.data[i].key;
                thirdpack.data.emplace_back(pack);
            }
            else if (data.data[i].eventType == CloudUpdateEventType)
            {
                // cloudupdate，云端资源需要添加或者更新
                std::string type = isLocalResource(data.data[i].key);
                if (type == "")
                {
                    LOG(ERROR) << "the resource: " << data.data[i].key << " is not the local resource";
                    continue;
                }
                KeyDatapack pack;
                std::string update_data;
                if (type == CameraDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    update_data = std::dynamic_pointer_cast<CameraInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == LoudspeakerDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    update_data = std::dynamic_pointer_cast<LoudspeakerInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == SensorDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    update_data = std::dynamic_pointer_cast<SensorInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == MicrophoneDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(devices_lock_);
                    update_data = std::dynamic_pointer_cast<MicrophoneInstance>(devices_[data.data[i].key])->Marshal();
                }
                else if (type == AbilityResourcetype)
                {
                    std::lock_guard<std::mutex> lock(abilities_lock_);
                    update_data = abilities_[data.data[i].key]->Marshal();
                }
                pack.data = update_data;
                pack.eventType = data.data[i].eventType;
                pack.key = data.data[i].key;
                thirdpack.data.emplace_back(pack);
            }
            else if (data.data[i].eventType == EdgeAddEventType)
            {
                std::string type = data.data[i].resourceType;
                if (type == AbilityResourcetype)
                {
                    this->AddAbilityInstance(data.data[i].data);
                }
                else if (type == CameraDeviceResourcetype || type == LoudspeakerDeviceResourcetype || type == SensorDeviceResourcetype || type == MicrophoneDeviceResourcetype)
                {
                    this->AddDeviceInstance(data.data[i].data);
                }
                else
                {
                    LOG(ERROR) << data.data[i].key << " not supply the resourcetype";
                }
            }
            else if (data.data[i].eventType == EdgeUpdateEventType)
            {
                std::string type = data.data[i].resourceType;
                if (type == AbilityResourcetype)
                {
                    this->UpdateAbilityInstance(data.data[i].data);
                }
                else if (type == CameraDeviceResourcetype || type == LoudspeakerDeviceResourcetype || type == SensorDeviceResourcetype || type == MicrophoneDeviceResourcetype)
                {
                    this->UpdateDeviceInstance(data.data[i].data);
                }
                else
                {
                    LOG(ERROR) << data.data[i].key << "not supply the resourcetype";
                }
            }
            else if (data.data[i].eventType == EdgeDeleteEventType)
            {
                std::string type = data.data[i].resourceType;
                if (type == AbilityResourcetype)
                {
                    this->DeleteAbilityInstance(data.data[i].key);
                }
                else if (type == CameraDeviceResourcetype || type == LoudspeakerDeviceResourcetype || type == SensorDeviceResourcetype || type == MicrophoneDeviceResourcetype)
                {
                    this->DeleteAbilityInstance(data.data[i].key);
                }
                else
                {
                    LOG(ERROR) << data.data[i].key << "not supply the resourcetype";
                }
            }
            else
            {
                LOG(ERROR) << "eventtype: " << data.data[i].eventType << " is unsupported";
                return;
            }
        }
        // clean the deleted resource
        {
            std::lock_guard<std::mutex> locker(nonlocal_lock_);
            for (auto &iter : record)
            {
                if (iter.second.size() != 0)
                {
                    for (auto &it : iter.second)
                    {
                        nonlocal_instances_.erase(it);
                    }
                }
            }
        }

        // send pack
        thirdpack.packageType = CloudSyncStepThree;
        thirdpack.hostname = this->hostname_;
        std::string thirdmessage = MarshalMessageStruct(thirdpack);
        LOG(INFO) << "send message to cloud , type : " << thirdpack.packageType;
        connection_->SendMessageToCloud(thirdmessage);
    }
    else if (data.packageType == CloudSyncAllOK)
    {
        LOG(INFO) << "cloud sync success";
    }
    else
    {
        LOG(ERROR) << "message type error";
    }
}

void ResourceManager::endMessageHandle(const KeyAndDataPackages &data)
{
    // only need to check whether is "ok"
    if (data.packageType == EndSync)
    {
        std::list<std::string> record;
        std::lock_guard<std::mutex> locker(nonlocal_lock_);
        // find the resource belong to hostname
        for (auto &iter : nonlocal_instances_)
        {
            if (iter.second->hostname == data.hostname)
            {
                record.push_back(iter.first);
            }
        }
        for (int i = 0; i < data.data.size(); i++)
        {
            auto resource = std::make_shared<NonLocalResource>();
            if (!UnMarshal(data.data[i].data, resource))
            {
                LOG(ERROR) << "unmarshal fail";
            }
            nonlocal_instances_[data.data[i].key] = resource;
            record.remove(data.data[i].key);
        }
        if (record.size() != 0)
        {
            for (auto &it : record)
            {
                nonlocal_instances_.erase(it);
            }
        }
        KeyAndDataPackages response;
        response.packageType = EndSyncAllOK;
        response.hostname = this->hostname_;
        auto response_ = MarshalMessageStruct(response);
        connection_->SendMessageToEnd(response_, data.hostname);
        LOG(INFO) << "receive end sync data from : " << data.hostname;
    }
    else if (data.packageType == EndSyncAllOK)
    {
        LOG(INFO) << "sync with host: " << data.hostname << " success";
    }
}

std::vector<std::string> ResourceManager::GetHardWareResourceList(std::string type)
{
    return this->hardware_manager_->GetHardwareResourceList(type);
}

void ResourceManager::RecvMessageHandle(const std::string &message)
{
    KeyAndDataPackages data;
    if (UnMarshalMessageStruct(message, data))
    {
        if (data.packageType == EndSync || data.packageType == EndSyncAllOK)
        {
            this->endMessageHandle(data);
        }
        else if (data.packageType == CloudSyncAllOK || data.packageType == CloudSyncStepTwo)
        {
            this->cloudMessageHandle(data);
        }
        else
        {
            LOG(ERROR) << "message type error";
        }
    }
    else
    {
        LOG(ERROR) << "message unmarshal error";
    }
}

void ResourceManager::InsertHardwareInfo(std::map<std::string, CameraHardware> &camera,
                                         std::map<std::string, AudioHardware> &mic,
                                         std::map<std::string, AudioHardware> &speaker)
{
    std::lock_guard<std::mutex> locker(devices_lock_);
    for (auto &iter : devices_)
    {
        if (iter.second->kind == CameraDeviceResourcetype)
        {
            if (camera.count(iter.second->GetHardwareIdentifier()) != 0)
            {
                auto change = iter.second->UpdateHardwareInfo(camera[iter.second->GetHardwareIdentifier()].toJson());
                if (change)
                {
                    Json::Value data;
                    StringToJson(iter.second->Marshal(), data);
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(data);
                }
                camera.erase(iter.second->GetHardwareIdentifier());
            }
        }
        else if (iter.second->kind == MicrophoneDeviceResourcetype)
        {
            if (mic.count(iter.second->GetHardwareIdentifier()) != 0)
            {
                auto change = iter.second->UpdateHardwareInfo(mic[iter.second->GetHardwareIdentifier()].toJson());
                if (change)
                {
                    Json::Value data;
                    StringToJson(iter.second->Marshal(), data);
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(data);
                }
                mic.erase(iter.second->GetHardwareIdentifier());
            }
        }
        else if (iter.second->kind == LoudspeakerDeviceResourcetype)
        {
            if (speaker.count(iter.second->GetHardwareIdentifier()) != 0)
            {
                auto change = iter.second->UpdateHardwareInfo(speaker[iter.second->GetHardwareIdentifier()].toJson());
                if (change)
                {
                    Json::Value data;
                    StringToJson(iter.second->Marshal(), data);
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(data);
                }
                speaker.erase(iter.second->GetHardwareIdentifier());
            }
        }
    }
    for (auto iter = camera.begin(); iter != camera.end();)
    {
    }
}

std::string ResourceManager::generateNonLocalFormat()
{
    KeyAndDataPackages data;
    data.hostname = this->hostname_;
    data.packageType = EndSync;
    std::lock_guard<std::mutex> locker1(this->devices_lock_);
    std::lock_guard<std::mutex> locker2(this->abilities_lock_);
    for (const auto &iter : abilities_)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto new_nonlocal = std::make_shared<NonLocalResource>();
        new_nonlocal->key = iter.first;
        new_nonlocal->kind = AbilityResourcetype;
        new_nonlocal->hostname = this->hostname_;
        // new_nonlocal.devicelist = iter.second->abilityinstancelist; //skip
        pack.data = Marshal(new_nonlocal);
        data.data.emplace_back(pack);
    }
    for (const auto &iter : devices_)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto new_nonlocal = std::make_shared<NonLocalResource>();
        new_nonlocal->key = iter.first;
        new_nonlocal->kind = iter.second->kind;
        new_nonlocal->hostname = this->hostname_;
        new_nonlocal->devicelist = iter.second->devicelist;
        pack.data = Marshal(new_nonlocal);
        data.data.emplace_back(pack);
    }
    return MarshalMessageStruct(data);
}

std::string ResourceManager::generateKVFormat()
{
    KeyAndDataPackages data;
    data.hostname = this->hostname_;
    data.packageType = CloudSyncStepOne;
    for (const auto &iter : this->versionRecord)
    {
        KeyDatapack kd;
        kd.key = iter.first;
        kd.version = iter.second;
        data.data.emplace_back(kd);
    }
    return MarshalMessageStruct(data);
    ;
}

void ResourceManager::getHostName()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        LOG(ERROR) << "get hostname fail";
    }
    else
    {
        hostname[sizeof(hostname) - 1] = '\0';
        hostname_ = std::string(hostname);
    }
}

void ResourceManager::cloudSyncThread()
{
    while (!connection_->JudgeCloudAddressExist())
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    if (!connection_->ConnectWithCloud())
    {
        LOG(ERROR) << "connect with cloud error";
        return;
    }
    while (true)
    {
        LOG(INFO) << "Cloud sync begin";
        this->RefreshKVRecord();
        std::string data = generateKVFormat();
        connection_->SendMessageToCloud(data);
        sleep(4);
    }
}

void ResourceManager::endSyncThread()
{
    while (!connection_->JudgeEndAddressExist())
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    if (!connection_->ConnectWithEnd())
    {
        LOG(ERROR) << "connect with end error";
        return;
    }
    while (true)
    {
        LOG(INFO) << "End sync begin";
        std::string data = generateNonLocalFormat();
        connection_->SendMessageToEnd(data, std::nullopt);
        sleep(4);
    }
}
