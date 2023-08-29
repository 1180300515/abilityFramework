/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: resource_manager.cpp
 * Description: resource_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/resource_manager.h"

#include <json/json.h>
#include <yaml-cpp/yaml.h>

#include <fstream>

#include "databasemgr/database_manager.h"
#include "resourcemgr/device_instance_camera.h"
#include "resourcemgr/device_instance_loudspeaker.h"
#include "resourcemgr/device_instance_microphone.h"
#include "resourcemgr/device_instance_sensor.h"
#include "resourcemgr/message_package_struct.h"
#include "utils/color.h"
#include "utils/global_var.h"
#include "utils/yaml_json_converter.h"

ResourceManager::ResourceManager() { DatabaseManager::getInstance(); }

bool ResourceManager::RegistCrd(const std::string &data, bool from_file)
{
    if (from_file) {
        return DatabaseManager::getInstance().RegistCrdFromFile(data);
    } else {
        return DatabaseManager::getInstance().RegistCrd(data);
    }
    return false;
}

bool ResourceManager::UnregistCrd(const std::string &name) { return DatabaseManager::getInstance().UnregistCrd(name); }

bool ResourceManager::AddAbilityInstance(const std::string &data, bool from_file)
{
    Json::Value instance_json;
    if (from_file) {
        YAML::Node instance_yaml;
        try {
            instance_yaml = YAML::LoadFile(data);
        } catch (YAML::ParserException e) {
            DLOG(ERROR) << "yaml is malformed.";
            return false;
        } catch (YAML::BadFile e) {
            DLOG(ERROR) << "file can't be load";
            return false;
        }
        if (!YamlToJson(instance_yaml, &instance_json)) {
            DLOG(ERROR) << "convert the yaml file in: " << data << " to json format fail";
            return false;
        }
    } else {
        Json::Reader reader;
        reader.parse(data, instance_json);
    }
    handleNamespace(&instance_json);
    if (DatabaseManager::getInstance().DBStoreAbilityInstance(instance_json)) {
        std::lock_guard<std::mutex> locker(abilities_lock_);
        auto resource = std::make_shared<AbilityInstanceInfo>();
        resource->FromJson(instance_json);
        std::string key = GetInstanceKey(instance_json);
        abilities_[key] = resource;
        DLOG(INFO) << "resource manager add ability instance : " << key;
        return true;
    } else {
        DLOG(ERROR) << "resource manager add ability instance fail";
        return false;
    }
    return false;
}

bool ResourceManager::UpdateAbilityInstance(const std::string &data)
{
    Json::Value instance_json;
    Json::Reader reader;
    reader.parse(data, instance_json);
    if (DatabaseManager::getInstance().DBUpdateAbilityInstance(instance_json)) {
        std::lock_guard<std::mutex> locker(abilities_lock_);
        std::string key = GetInstanceKey(instance_json);
        abilities_[key]->updateAbility(instance_json);
        DLOG(INFO) << "resource manager update ability instance : " << key;
        return true;
    } else {
        DLOG(ERROR) << "resource manager update ability instance fail";
        return false;
    }
    return false;
}

bool ResourceManager::DeleteAbilityInstance(const std::string &key)
{
    if (abilities_.count(key) == 0) {
        DLOG(INFO) << "the instance : " << key << " is not exist";
        return true;
    } else {
        if (DatabaseManager::getInstance().DBDeleteAbilityInstance(key)) {
            std::lock_guard<std::mutex> locker(abilities_lock_);
            abilities_.erase(key);
            DLOG(INFO) << "resource manager delete ability instance : " << key;
            return true;
        } else {
            DLOG(ERROR) << "resource manager delete ability instance fail";
        }
    }

    return false;
}

bool ResourceManager::AddDeviceInstance(const std::string &data, bool from_file)
{
    Json::Value instance_json;
    if (from_file) {
        YAML::Node instance_yaml;
        try {
            instance_yaml = YAML::LoadFile(data);
        } catch (YAML::ParserException e) {
            DLOG(ERROR) << "yaml is malformed.";
            return false;
        } catch (YAML::BadFile e) {
            DLOG(ERROR) << "file can't be load";
            return false;
        }
        if (!YamlToJsonForInstance(instance_yaml, &instance_json)) {
            DLOG(ERROR) << "convert the yaml file in: " << data << " to json format fail";
            return false;
        }
    } else {
        Json::Reader reader;
        reader.parse(data, instance_json);
    }
    if (!handleHostname(&instance_json)) {
        return false;
    }
    handleNamespace(&instance_json);
    if (!DatabaseManager::getInstance().validateJson(instance_json)) {
        return false;
    }

    std::lock_guard<std::mutex> locker(devices_lock_);
    std::string device_kind = GetInstanceKind(instance_json);
    std::shared_ptr<DeviceInstanceInfo> new_device;
    if (device_kind == CameraDeviceResourcetype) {
        new_device = std::make_shared<CameraInstance>();
        new_device->FromJson(instance_json);
    } else if (device_kind == LoudspeakerDeviceResourcetype) {
        new_device = std::make_shared<LoudspeakerInstance>();
        new_device->FromJson(instance_json);
    } else if (device_kind == MicrophoneDeviceResourcetype) {
        new_device = std::make_shared<MicrophoneInstance>();
        new_device->FromJson(instance_json);
    } else if (device_kind == SensorDeviceResourcetype) {
        new_device = std::make_shared<SensorInstance>();
        new_device->FromJson(instance_json);
        auto key = GetInstanceKey(instance_json);
        if (!DatabaseManager::getInstance().DBStoreDeviceInstance(new_device->ToJson())) {
            DLOG(ERROR) << "resource manager add device instance : " << key << " type: " << device_kind << " fail";
            return false;
        }

        devices_[key] = new_device;
        DLOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
        return true;
    } else {
        DLOG(ERROR) << "unkonown resource type : " << device_kind;
        return false;
    }
    auto key = GetInstanceKey(instance_json);
    std::string id = new_device->GetHardwareIdentifier();
    auto hw = this->hardware_->GetHardware(id, key, new_device->kind);
    if (hw != std::nullopt) {
        new_device->UpdateHardwareInfo(hw.value());
        if (!DatabaseManager::getInstance().DBStoreDeviceInstance(new_device->ToJson())) {
            return false;
        }
        this->hardware_->SetMap(id, key);
    } else {
        if (!DatabaseManager::getInstance().DBStoreDeviceInstance(new_device->ToJson())) {
            return false;
        }
    }
    devices_[key] = new_device;
    DLOG(INFO) << "resource manager add device instance : " << key << " type: " << device_kind;
    return true;
}

bool ResourceManager::UpdateDeviceInstance(const std::string &data)
{
    Json::Value instance_json;
    Json::Reader reader;
    reader.parse(data, instance_json);
    if (DatabaseManager::getInstance().DBUpdateDeviceInstance(instance_json)) {
        std::lock_guard<std::mutex> locker(abilities_lock_);
        std::string key = GetInstanceKey(instance_json);
        std::string device_kind = GetInstanceKind(instance_json);
        if (device_kind == CameraDeviceResourcetype) {
            auto new_device = std::dynamic_pointer_cast<CameraInstance>(devices_[key]);
            new_device->updateInstance(instance_json);
            DLOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
            return true;
        } else if (device_kind == LoudspeakerDeviceResourcetype) {
            auto new_device = std::dynamic_pointer_cast<LoudspeakerInstance>(devices_[key]);
            new_device->updateInstance(instance_json);
            DLOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
            return true;
        } else if (device_kind == MicrophoneDeviceResourcetype) {
            auto new_device = std::dynamic_pointer_cast<MicrophoneInstance>(devices_[key]);
            new_device->updateInstance(instance_json);
            DLOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
            return true;
        } else if (device_kind == SensorDeviceResourcetype) {
            auto new_device = std::dynamic_pointer_cast<SensorInstance>(devices_[key]);
            new_device->updateInstance(instance_json);
            DLOG(INFO) << "resource manager update device instance : " << key << " type: " << device_kind;
            return true;
        } else {
            DLOG(ERROR) << "unkonown resource type : " << device_kind;
            return false;
        }
    } else {
        DLOG(ERROR) << "resource manager update device instance fail";
        return false;
    }
    return false;
}

bool ResourceManager::DeleteDeviceInstance(const std::string &key)
{
    if (deleteDeviceInstance(key)) {
        this->hardware_->DeleteMap(key);
        return true;
    }
    return false;
}

std::string ResourceManager::isLocalResource(std::string key)
{
    std::lock_guard<std::mutex> locker1(this->devices_lock_);
    std::lock_guard<std::mutex> locker2(this->abilities_lock_);
    if (abilities_.count(key) != 0) {
        return AbilityResourcetype;
    } else if (devices_.count(key) != 0) {
        return devices_[key]->kind;
    } else {
        return "";
    }
    return std::string();
}

bool ResourceManager::AbilityExistJudge(const std::string &key)
{
    std::string wait_for_check = "default/" + key;
    DLOG(INFO) << L_BLUE << "check ability exist : " << wait_for_check << NONE;
    for (const auto &iter : abilities_) {
        if (wait_for_check == iter.first) {
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
    DatabaseManager::getInstance().DBGetAbilityInstances(&data);
    if (data.size() != 0) {
        for (const auto &iter : data) {
            DLOG(INFO) << "add resource : " << iter.first;
            auto new_ability = std::make_shared<AbilityInstanceInfo>();
            new_ability->UnMarshal(iter.second);
            this->abilities_[iter.first] = new_ability;
        }
    } else {
        DLOG(INFO) << "ability instance in database is empty";
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(CameraDeviceResourcetype, &data);
    if (data.size() != 0) {
        for (const auto &iter : data) {
            auto new_device = std::make_shared<CameraInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    } else {
        DLOG(INFO) << "camera device instance in database is empty";
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(MicrophoneDeviceResourcetype, &data);
    if (data.size() != 0) {
        for (const auto &iter : data) {
            auto new_device = std::make_shared<MicrophoneInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    } else {
        DLOG(INFO) << "microphone device instance in database is empty";
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(LoudspeakerDeviceResourcetype, &data);
    if (data.size() != 0) {
        for (const auto &iter : data) {
            auto new_device = std::make_shared<LoudspeakerInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    } else {
        DLOG(INFO) << "loudspeaker device instance in database is empty";
    }
    data.clear();
    DatabaseManager::getInstance().DBGetDeviceInstances(SensorDeviceResourcetype, &data);
    if (data.size() != 0) {
        for (const auto &iter : data) {
            auto new_device = std::make_shared<SensorInstance>();
            new_device->UnMarshal(iter.second);
            this->devices_[iter.first] = new_device;
        }
    } else {
        DLOG(INFO) << "sensor device instance in database is empty";
    }
}

void ResourceManager::Init(bool cleandb)
{
    DLOG(INFO) << L_GREEN << "init resource manager" << NONE;
    getHostName();

    DatabaseManager::getInstance().Init(this->hostname_, cleandb);
    LoadLocalResourceFromDB();

    this->hardware_ = std::make_shared<HardwareScan>();
    std::shared_ptr<ResourceManager> p(this);
    this->hardware_->Init(this->hostname_);

    this->hardware_manager_ = std::make_shared<HardwareResourceManager>();
    this->hardware_manager_->Init(p, this->hostname_);
}

void ResourceManager::Run()
{
    autoGenerateCR();
    periodic_scan_thread = std::thread(&ResourceManager::periodicScanThread, this);
}

void ResourceManager::Wait() { periodic_scan_thread.join(); }

std::string ResourceManager::GetHardwareDeviceInfo(bool format) { return hardware_->GetHardwareDeviceProfile(format); }

std::shared_ptr<DeviceInstanceInfo> ResourceManager::GetDeviceInstance(const std::string &key)
{
    std::lock_guard<std::mutex> locker(devices_lock_);
    if (devices_.count(key) != 0) {
        return devices_[key];
    }
    return std::shared_ptr<DeviceInstanceInfo>();
}

std::shared_ptr<AbilityInstanceInfo> ResourceManager::GetAbilityInstance(const std::string &key)
{
    std::lock_guard<std::mutex> locker(abilities_lock_);
    if (abilities_.count(key) != 0) {
        return abilities_[key];
    }
    return std::shared_ptr<AbilityInstanceInfo>();
}

void ResourceManager::Print()
{
    std::lock_guard<std::mutex> locker1(abilities_lock_);
    std::lock_guard<std::mutex> locker2(devices_lock_);
    for (const auto &iter : abilities_) {
        std::cout << RED << iter.first << NONE << std::endl;
    }
    for (const auto &iter : devices_) {
        std::cout << RED << iter.first << NONE << std::endl;
    }
}

void ResourceManager::PrintMatchMap() { hardware_->PrintMap(); }

std::vector<AbilityInfoExtract> ResourceManager::GetAbilityInfoExtractList()
{
    std::vector<AbilityInfoExtract> result;
    std::lock_guard<std::mutex> locker(abilities_lock_);
    for (const auto &iter : abilities_) {
        AbilityInfoExtract info;
        info.name = StripSlashPrefix(iter.first);
        info.depends.abilities = iter.second->depends.abilities;
        info.depends.devices = iter.second->depends.devices;
        result.emplace_back(info);
    }
    return result;
}

std::vector<std::string> ResourceManager::GetHardWareResourceList(std::string type) { return this->hardware_manager_->GetHardwareResourceList(type); }

void ResourceManager::getHostName()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        DLOG(ERROR) << "get hostname fail";
    } else {
        hostname[sizeof(hostname) - 1] = '\0';
        hostname_ = std::string(hostname);
    }
}

void ResourceManager::handleNamespace(Json::Value *instance_json)
{
    if (!(*instance_json)["metadata"].isMember("namespace")) {
        (*instance_json)["metadata"]["namespace"] = "default";
    }
}

bool ResourceManager::handleHostname(Json::Value *instance_json)
{
    if ((*instance_json)["spec"].isMember("hostname")) {
        if ((*instance_json)["spec"]["hostname"] != this->hostname_ && (*instance_json)["spec"]["hostname"] != "") {
            DLOG(ERROR) << "the " << (*instance_json)["spec"]["hostname"] << " is not the local hostname";
            return false;
        }
    }
    (*instance_json)["spec"]["hostname"] = this->hostname_;
    return true;
}

bool ResourceManager::deleteDeviceInstance(const std::string &key)
{
    if (devices_.count(key) == 0) {
        DLOG(INFO) << "the instance : " << key << " is not exist";
        return true;
    } else {
        if (DatabaseManager::getInstance().DBDelteDeviceInstance(key)) {
            std::lock_guard<std::mutex> locker(devices_lock_);
            devices_.erase(key);

            DLOG(INFO) << "resource manager delete device instance : " << key;
            return true;
        } else {
            DLOG(ERROR) << "resource manager delete device instance fail";
            return false;
        }
    }
    return false;
}

void ResourceManager::resourceMatching()
{
    DLOG(INFO) << "begin resource match with hardware";
    std::vector<std::string> wait_for_delete;
    std::lock_guard<std::mutex> locker(devices_lock_);
    for (auto &iter : this->devices_) {
        std::string id = iter.second->GetHardwareIdentifier();
        // check hardware is exist or not
        auto old_key = hardware_->GetMatchedKey(id);
        if (old_key == "delete") {
            if (hardware_->isAutogenInstance(iter.first)) {
                wait_for_delete.emplace_back(iter.first);
                continue;
            } else {
                iter.second->EraseHardwareInfo();
                DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
                hardware_->DeleteMap(id);
                continue;
            }
        }
        if (old_key == "not-exist") {
            continue;
        }
        // update or insert hardware into instance
        auto hw = this->hardware_->GetHardware(id, iter.first, iter.second->kind);
        if (hw == std::nullopt) {
            continue;
        }
        auto change = iter.second->UpdateHardwareInfo(hw.value());
        if (change) {
            DatabaseManager::getInstance().DBUpdateDeviceInstance(iter.second->ToJson());
            DLOG(INFO) << "device instance : " << iter.first << " update hardware info";
        }
        if (hardware_->isAutogenInstance(old_key)) {
            wait_for_delete.emplace_back(old_key);
        }
        this->hardware_->SetMap(id, iter.first);
    }
    for (const auto &iter : wait_for_delete) {
        DatabaseManager::getInstance().DBDelteDeviceInstance(iter);
        devices_.erase(iter);
        DLOG(INFO) << "resource manager delete autogen CR : " << iter;
    }
    DLOG(INFO) << "finish match";
}

void ResourceManager::autoGenerateCR()
{
    DLOG(INFO) << "begin autogen CR";
    std::vector<CameraHardware> camera;
    std::vector<AudioHardware> mic;
    std::vector<AudioHardware> speaker;
    hardware_->UnmatchedHardware(&camera, &mic, &speaker);
    for (const auto &iter : camera) {
        CameraInstance instance;
        instance.apiVersion = "stable.example.com/v1";
        instance.kind = CameraDeviceResourcetype;
        int count = hardware_->GenerateSerialNumber("camera");
        instance.metadata.name = "Autogen-Camera-" + std::to_string(count);
        instance.metadata.namespace_name = "default";
        instance.spec.version = "1.0";
        instance.spec.hostname = hostname_;
        instance.spec.kind = "camera";
        instance.spec.hardwareidentifier = iter.bus_info;
        // properties part
        instance.spec.properties.vendor = "unknown";
        instance.spec.properties.resolution = "unknown";
        instance.spec.properties.location = "unknown";
        instance.spec.properties.wideAngle = 0;
        instance.spec.properties.focusMethod = "unknown";
        instance.spec.properties.telephoto = false;
        instance.spec.properties.interface = "unknown";

        instance.spec.properties.devicePath = iter.device_path;
        instance.spec.properties.driverName = iter.driver;
        instance.spec.properties.card = iter.card;
        instance.spec.properties.busInfo = iter.bus_info;
        for (int i = 0; i < iter.formats.size(); i++) {
            instance.spec.properties.supportFormat.emplace_back(iter.formats[i]);
        }
        // status part
        instance.status.occupancy = false;
        std::string data = instance.Marshal();
        if (!AddDeviceInstance(data)) {
            DLOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            continue;
        }
        hardware_->SetMap(iter.bus_info, instance.metadata.namespace_name + "/" + instance.metadata.name);
    }
    for (const auto &iter : mic) {
        MicrophoneInstance instance;
        instance.apiVersion = "stable.example.com/v1";
        instance.kind = MicrophoneDeviceResourcetype;
        int count = hardware_->GenerateSerialNumber("mic");
        instance.metadata.name = "Autogen-Mic-" + std::to_string(count);
        instance.metadata.namespace_name = "default";

        instance.spec.version = "1.0";
        instance.spec.hostname = hostname_;
        instance.spec.kind = "microphone";
        instance.spec.hardwareidentifier = iter.name;
        // properties part
        instance.spec.properties.interface = "unknown";
        instance.spec.properties.sampleRates = "unknown";

        instance.spec.properties.hardwareName = iter.name;
        instance.spec.properties.description = iter.description;
        instance.spec.properties.sampleRate = iter.sampleRate;
        instance.spec.properties.channels = iter.channels;
        instance.spec.properties.format = iter.format;
        instance.spec.properties.cardID = iter.cardID;
        instance.spec.properties.deviceID = iter.deviceID;

        // status part
        instance.status.occupancy = false;

        std::string data = instance.Marshal();
        if (!AddDeviceInstance(data)) {
            DLOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            continue;
        }

        // DLOG(INFO) << "add device instance : " << instance.metadata.name << "
        // success";
        hardware_->SetMap(iter.name, instance.metadata.namespace_name + "/" + instance.metadata.name);
    }
    for (const auto &iter : speaker) {
        LoudspeakerInstance instance;
        instance.apiVersion = "stable.example.com/v1";
        instance.kind = LoudspeakerDeviceResourcetype;
        int count = hardware_->GenerateSerialNumber("speaker");
        instance.metadata.name = "Autogen-Loudspeaker-" + std::to_string(count);
        instance.metadata.namespace_name = "default";

        instance.spec.version = "1.0";
        instance.spec.hostname = hostname_;
        instance.spec.kind = "loudspeaker";
        instance.spec.hardwareidentifier = iter.name;
        // properties part
        instance.spec.properties.sampleRates = "unknown";
        instance.spec.properties.channelNumber = 0;
        instance.spec.properties.bitWidth = 0;
        instance.spec.properties.interface = "unknown";

        instance.spec.properties.hardwareName = iter.name;

        instance.spec.properties.hardwareName = iter.name;
        instance.spec.properties.description = iter.description;
        instance.spec.properties.sampleRate = iter.sampleRate;
        instance.spec.properties.channels = iter.channels;
        instance.spec.properties.format = iter.format;
        instance.spec.properties.cardID = iter.cardID;
        instance.spec.properties.deviceID = iter.deviceID;
        // status part
        instance.status.occupancy = false;

        std::string data = instance.Marshal();
        if (!AddDeviceInstance(data)) {
            DLOG(ERROR) << "autogen CR: " << instance.metadata.name << " add fail";
            continue;
        }
        // DLOG(INFO) << "add device instance : " << instance.metadata.name << "
        // success";
        hardware_->SetMap(iter.name, instance.metadata.namespace_name + "/" + instance.metadata.name);
    }
    DLOG(INFO) << "finish autogen CR";
}

void ResourceManager::periodicScanThread()
{
    while (true) {
        std::this_thread::sleep_for(std::chrono::minutes(10));
        hardware_->LocalHardwareScan();
        resourceMatching();
        autoGenerateCR();
    }
}
