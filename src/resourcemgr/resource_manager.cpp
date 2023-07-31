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
#include "color.h"

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
        Json::Reader reader;
        reader.parse(data, instance_json);
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

bool ResourceManager::UpdateAbilityInstance(const std::string &data)
{
    Json::Value instance_json;
    Json::Reader reader;
    reader.parse(data, instance_json);
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
            if (!addDeviceInstance(instance_json))
            {
                auto key = GetInstanceKey(instance_json);
                DatabaseManager::getInstance().DBDelteDeviceInstance(key);
                return false;
            }
            else
            {
                return true;
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
        Json::Reader reader;
        reader.parse(data, instance_json);
        if (DatabaseManager::getInstance().DBStoreDeviceInstance(instance_json))
        {
            if (!addDeviceInstance(instance_json))
            {
                auto key = GetInstanceKey(instance_json);
                DatabaseManager::getInstance().DBDelteDeviceInstance(key);
                return false;
            }
            else
            {
                return true;
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

bool ResourceManager::UpdateDeviceInstance(const std::string &data)
{
    Json::Value instance_json;
    Json::Reader reader;
    reader.parse(data, instance_json);
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
    return false;
}

bool ResourceManager::DeleteDeviceInstance(const std::string &key)
{
    if (deleteDeviceInstance(key))
    {
        this->hardware_->DeleteMap(key);
        return true;
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
    std::string wait_for_check = "default/" + key;
    LOG(INFO) << L_BLUE << "check ability exist : " << wait_for_check << NONE;
    for (const auto &iter : abilities_)
    {
        if (wait_for_check == iter.first)
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

void ResourceManager::LoadLocalResourceFromDB()
{
    std::map<std::string, std::string> data;
    DatabaseManager::getInstance().DBGetAbilityInstances(data);
    if (data.size() != 0)
    {
        for (const auto &iter : data)
        {
            LOG(INFO) << "add resource : " << iter.first;
            auto new_ability = std::make_shared<AbilityInstanceInfo>();
            new_ability->UnMarshal(iter.second);
            this->abilities_[iter.first] = new_ability;
        }
    }
    else
    {
        LOG(INFO) << "ability instance in database is empty";
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
    else
    {
        LOG(INFO) << "camera device instance in database is empty";
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
    else
    {
        LOG(INFO) << "microphone device instance in database is empty";
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
    else
    {
        LOG(INFO) << "loudspeaker device instance in database is empty";
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
    else
    {
        LOG(INFO) << "sensor device instance in database is empty";
    }
}

void ResourceManager::Init(bool cleandb)
{
    LOG(INFO) << L_GREEN << "init resource manager" << NONE;
    getHostName();

    DatabaseManager::getInstance().Init(this->hostname_, cleandb);
    LoadLocalResourceFromDB();

    this->hardware_ = std::make_shared<HardwareScan>();
    std::shared_ptr<ResourceManager> p(this);
    this->hardware_->Init(p, this->hostname_);

    this->hardware_manager_ = std::make_shared<HardwareResourceManager>();
    this->hardware_manager_->Init(p, this->hostname_);
}

void ResourceManager::Run()
{
    this->hardware_->Run();
}

void ResourceManager::Wait()
{
    this->hardware_->Wait();
}

std::string ResourceManager::GetHardwareDeviceInfo(bool format)
{
    return hardware_->GetHardwareDeviceInfo(format);
}

std::shared_ptr<DeviceInstanceInfo> ResourceManager::GetDeviceInstance(const std::string &key)
{
    std::lock_guard<std::mutex> locker(devices_lock_);
    if (devices_.count(key) != 0)
    {
        return devices_[key];
    }
    return std::shared_ptr<DeviceInstanceInfo>();
}

std::shared_ptr<AbilityInstanceInfo> ResourceManager::GetAbilityInstance(const std::string &key)
{
    std::lock_guard<std::mutex> locker(abilities_lock_);
    if (abilities_.count(key) != 0)
    {
        return abilities_[key];
    }
    return std::shared_ptr<AbilityInstanceInfo>();
}

void ResourceManager::Print()
{
    std::lock_guard<std::mutex> locker1(abilities_lock_);
    std::lock_guard<std::mutex> locker2(devices_lock_);
    for (const auto &iter : abilities_)
    {
        std::cout << RED << iter.first << NONE << std::endl;
    }
    for (const auto &iter : devices_)
    {
        std::cout << RED << iter.first << NONE << std::endl;
    }
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

std::vector<std::string> ResourceManager::GetHardWareResourceList(std::string type)
{
    return this->hardware_manager_->GetHardwareResourceList(type);
}

void ResourceManager::InsertHardwareInfo(const std::map<std::string, CameraHardware> &camera,
                                         const std::map<std::string, AudioHardware> &mic,
                                         const std::map<std::string, AudioHardware> &speaker,
                                         std::map<std::string, std::string> &record)
{
    LOG(INFO) << "begin insert the hardware into the matched instance";
    std::lock_guard<std::mutex> locker(devices_lock_);
    std::vector<std::string> wait_for_delete;
    for (const auto &iter : devices_)
    {
        if (this->hardware_->isAutogenInstance(iter.first))
        {
            continue;
        }
        auto hwid = iter.second->GetHardwareIdentifier();
        if (iter.second->kind == CameraDeviceResourcetype)
        {
            if (camera.count(hwid) != 0 && record.count(hwid) == 0)
            {
                // 第一次匹配到，而且尚未自动生成
                auto change = iter.second->UpdateHardwareInfo(camera.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "camera device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
            else if (camera.count(hwid) != 0 && record[hwid] == iter.first)
            {
                // 已经与instance匹配的hardware,只需要更新
                // LOG(INFO) << "camera hardware id : \"" << hwid << "\" match with camera device instance : " << iter.first;
                auto change = iter.second->UpdateHardwareInfo(camera.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "camera device instance : " << iter.first << " update hardware info";
                }
            }
            else if (camera.count(hwid) != 0 && this->hardware_->isAutogenInstance(record[hwid]))
            {
                // 匹配到，但是该硬件之前已经自动生成了
                DatabaseManager::getInstance().DBDelteDeviceInstance(record[hwid]);
                wait_for_delete.emplace_back(record[hwid]);
                auto change = iter.second->UpdateHardwareInfo(camera.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "camera device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
        }
        else if (iter.second->kind == MicrophoneDeviceResourcetype)
        {
            if (mic.count(hwid) != 0 && record.count(hwid) == 0)
            {
                // 第一次匹配到，而且尚未自动生成
                auto change = iter.second->UpdateHardwareInfo(mic.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "mic device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
            else if (mic.count(hwid) != 0 && record[hwid] == iter.first)
            {
                // 已经与instance匹配的hardware,只需要更新
                // LOG(INFO) << "camera hardware id : \"" << hwid << "\" match with camera device instance : " << iter.first;
                auto change = iter.second->UpdateHardwareInfo(mic.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "camera device instance : " << iter.first << " update hardware info";
                }
            }
            else if (mic.count(hwid) != 0 && this->hardware_->isAutogenInstance(record[hwid]))
            {
                // 匹配到，但是该硬件之前已经自动生成了
                DatabaseManager::getInstance().DBDelteDeviceInstance(record[hwid]);
                wait_for_delete.emplace_back(record[hwid]);
                auto change = iter.second->UpdateHardwareInfo(mic.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "camera device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
        }
        else if (iter.second->kind == LoudspeakerDeviceResourcetype)
        {
            if (speaker.count(hwid) != 0 && record.count(hwid) == 0)
            {
                // 第一次匹配到，而且尚未自动生成
                auto change = iter.second->UpdateHardwareInfo(speaker.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "speaker device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
            else if (speaker.count(hwid) != 0 && record[hwid] == iter.first)
            {
                // 已经与instance匹配的hardware,只需要更新
                // LOG(INFO) << "speaker hardware id : \"" << hwid << "\" match with speaker device instance : " << iter.first;
                auto change = iter.second->UpdateHardwareInfo(speaker.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "speaker device instance : " << iter.first << " update hardware info";
                }
            }
            else if (speaker.count(hwid) != 0 && this->hardware_->isAutogenInstance(record[hwid]))
            {
                // 匹配到，但是该硬件之前已经自动生成了
                DatabaseManager::getInstance().DBDelteDeviceInstance(record[hwid]);
                wait_for_delete.emplace_back(record[hwid]);
                auto change = iter.second->UpdateHardwareInfo(speaker.at(hwid).toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                    LOG(INFO) << "speaker device instance : " << iter.first << " update hardware info";
                }
                record[hwid] = iter.first;
            }
        }
    }
    for (const auto &iter : wait_for_delete)
    {
        this->devices_.erase(iter);
        LOG(INFO) << "resource manager delete autogen CR : " << iter;
    }
    LOG(INFO) << "finish insert";
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

bool ResourceManager::addDeviceInstance(const Json::Value &instance_json)
{
    std::lock_guard<std::mutex> locker(devices_lock_);
    std::string device_kind = GetInstanceKind(instance_json);
    if (device_kind == CameraDeviceResourcetype)
    {
        auto new_device = std::make_shared<CameraInstance>();
        new_device->FromJson(instance_json);
        auto key = GetInstanceKey(instance_json);
        std::string id = new_device->GetHardwareIdentifier();
        if (id != "")
        {
            auto hw = this->hardware_->GetCameraHardware(id, key);
            if (hw != std::nullopt)
            {
                auto old_key = this->hardware_->GetMatchedKey(id);
                if (old_key != "")
                {
                    // need to delete the autogen instance
                    if (DatabaseManager::getInstance().DBDelteDeviceInstance(old_key))
                    {
                        devices_.erase(old_key);
                        LOG(INFO) << "resource manager delete device instance : " << old_key;
                    }
                    else
                    {
                        LOG(ERROR) << "delete fail";
                        return false;
                    }
                }
                auto change = new_device->UpdateHardwareInfo(hw.value().toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(new_device->ToJson());
                    LOG(INFO) << "camera device instance : " << key << " update hardware info";
                }
                this->hardware_->SetMap(id, key);
            }
        }
        devices_[key] = new_device;
        LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
        return true;
    }
    else if (device_kind == LoudspeakerDeviceResourcetype)
    {
        auto new_device = std::make_shared<LoudspeakerInstance>();
        new_device->FromJson(instance_json);
        auto key = GetInstanceKey(instance_json);
        std::string id = new_device->GetHardwareIdentifier();
        if (id != "")
        {
            auto hw = this->hardware_->GetSpeakerHardware(id, key);
            if (hw != std::nullopt)
            {
                auto old_key = this->hardware_->GetMatchedKey(id);
                if (old_key != "")
                {
                    // need to delete the autogen instance
                    if (DatabaseManager::getInstance().DBDelteDeviceInstance(old_key))
                    {
                        devices_.erase(old_key);
                        LOG(INFO) << "resource manager delete device instance : " << old_key;
                    }
                    else
                    {
                        LOG(ERROR) << "delete fail";
                        return false;
                    }
                }
                auto change = new_device->UpdateHardwareInfo(hw.value().toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(new_device->ToJson());
                    LOG(INFO) << "speaker device instance : " << key << " update hardware info";
                }
                this->hardware_->SetMap(id, key);
            }
        }
        devices_[key] = new_device;
        LOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
        return true;
    }
    else if (device_kind == MicrophoneDeviceResourcetype)
    {
        auto new_device = std::make_shared<MicrophoneInstance>();
        new_device->FromJson(instance_json);
        auto key = GetInstanceKey(instance_json);
        std::string id = new_device->GetHardwareIdentifier();
        if (id != "")
        {
            auto hw = this->hardware_->GetMicHardware(id, key);
            if (hw != std::nullopt)
            {
                auto old_key = this->hardware_->GetMatchedKey(id);
                if (old_key != "")
                {
                    // need to delete the autogen instance
                    if (DatabaseManager::getInstance().DBDelteDeviceInstance(old_key))
                    {
                        devices_.erase(old_key);
                        LOG(INFO) << "resource manager delete device instance : " << old_key;
                    }
                    else
                    {
                        LOG(ERROR) << "delete fail";
                        return false;
                    }
                }
                auto change = new_device->UpdateHardwareInfo(hw.value().toJson());
                if (change)
                {
                    DatabaseManager::getInstance().DBUpdateDeviceInstance(new_device->ToJson());
                    LOG(INFO) << "mic device instance : " << key << " update hardware info";
                }
                this->hardware_->SetMap(id, key);
            }
        }
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

bool ResourceManager::deleteDeviceInstance(const std::string &key)
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
            return false;
        }
    }
    return false;
}
