#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <dirent.h>
#include <list>

#include "glog/logging.h"
#include "yaml-cpp/yaml.h"

#include "controller/controller.h"
#include "discoverymanager/discmgr_interface.h"

std::shared_ptr<Controller> Controller::getptr()
{
    return shared_from_this();
}

void Controller::SetConnection()
{
    connection_ = std::make_shared<ConnectionManager>(getptr());
    LOG(INFO) << "init connection manager success";
}

void Controller::LoadResource()
{
    if (!dbManager::getInstance().DBGetDeviceInstances(camera_instances))
    {
        LOG(ERROR) << "get all cameradevice error ";
    }
    if (!dbManager::getInstance().DBGetDeviceInstances(loudspeaker_instances))
    {
        LOG(ERROR) << "get all loudspeakerdevice error ";
    }
    if (!dbManager::getInstance().DBGetDeviceInstances(sensor_instances))
    {
        LOG(ERROR) << "get all sensordevice error ";
    }
    if (!dbManager::getInstance().DBGetDeviceInstances(mic_instances))
    {
        LOG(ERROR) << "get all micphonedevice error ";
    }
}

void Controller::SetEdgeAddressRecord(std::map<std::string, std::string> record)
{
    // init the connection manager
    if (connection_ == nullptr)
    {
        SetConnection();
    }
    connection_->SetEdgeAddressRecord(record);
    LOG(INFO) << "set non-native edge information success";
}

std::string Controller::isLocalResource(std::string key)
{
    std::lock_guard<std::mutex> locker1(camera_lock_);
    std::lock_guard<std::mutex> locker2(loudspeaker_lock_);
    std::lock_guard<std::mutex> locker3(sensor_lock_);
    std::lock_guard<std::mutex> locker4(mic_lock_);
    if (camera_instances.find(key) != camera_instances.end())
    {
        return CameraDeviceResourcetype;
    }
    if (loudspeaker_instances.find(key) != loudspeaker_instances.end())
    {
        return LoudspeakerDeviceResourcetype;
    }
    if (sensor_instances.find(key) != sensor_instances.end())
    {
        return SensorDeviceResourcetype;
    }
    if (mic_instances.find(key) != mic_instances.end())
    {
        return MicrophoneDeviceResourcetype;
    }
    return "";
}

void Controller::Run()
{
    LoadResource();
    // register callback
    connection_->RegisterCloudCallback(std::bind(&Controller::HandleResourceSync, getptr(), std::placeholders::_1));
    connection_->RegisterEdgeCallback(std::bind(&Controller::HandleResourceSyncEdge, getptr(), std::placeholders::_1));
    connection_->Run();
}

void Controller::HandleResourceSync(KeyAndDataPackages data)
{
    KeyAndDataPackages thirdpack;
    LOG(INFO) << "The synchandle function is called back";
    if (data.packageType != SyncStepTwo)
    {
        LOG(ERROR) << "message type error";
        return;
    }
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
                std::lock_guard<std::mutex> lock(camera_lock_);
                pack_version = camera_instances[data.data[i].key]->getInstanceVersion();
                update_data = camera_instances[data.data[i].key]->Marshal();
            }
            else if (type == LoudspeakerDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                pack_version = loudspeaker_instances[data.data[i].key]->getInstanceVersion();
                update_data = loudspeaker_instances[data.data[i].key]->Marshal();
            }
            else if (type == SensorDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(sensor_lock_);
                pack_version = sensor_instances[data.data[i].key]->getInstanceVersion();
                update_data = sensor_instances[data.data[i].key]->Marshal();
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
                std::lock_guard<std::mutex> lock(camera_lock_);
                update_data = camera_instances[data.data[i].key]->Marshal();
            }
            else if (type == LoudspeakerDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                update_data = loudspeaker_instances[data.data[i].key]->Marshal();
            }
            else if (type == SensorDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(sensor_lock_);
                update_data = sensor_instances[data.data[i].key]->Marshal();
            }
            pack.data = update_data;
            pack.eventType = data.data[i].eventType;
            pack.key = data.data[i].key;
            thirdpack.data.emplace_back(pack);
        }
        else if (data.data[i].eventType == EdgeAddEventType)
        {
            std::string type = data.data[i].resourceType;
            if (type == CameraDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(camera_lock_);
                auto resource = std::make_shared<CameraInstance>();
                resource->UnMarshal(data.data[i].data);
                camera_instances[data.data[i].key] = resource;
            }
            else if (type == LoudspeakerDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                auto resource = std::make_shared<LoudspeakerInstance>();
                resource->UnMarshal(data.data[i].data);
                loudspeaker_instances[data.data[i].key] = resource;
            }
            else if (type == SensorDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(sensor_lock_);
                auto resource = std::make_shared<SensorInstance>();
                resource->UnMarshal(data.data[i].data);
                sensor_instances[data.data[i].key] = resource;
            }
            else
            {
                LOG(ERROR) << data.data[i].key << " not supply the resourcetype";
            }
            // 更新数据库
            if (!dbManager::getInstance().DBStoreDeviceInstances(data.data[i].data))
            {
                LOG(ERROR) << "add resource fail :(key: " << data.data[i].key << ")";
                if (type == CameraDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(camera_lock_);
                    camera_instances.erase(data.data[i].key);
                }
                else if (type == LoudspeakerDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                    loudspeaker_instances.erase(data.data[i].key);
                }
                else if (type == SensorDeviceResourcetype)
                {
                    std::lock_guard<std::mutex> lock(sensor_lock_);
                    sensor_instances.erase(data.data[i].key);
                }
            }
        }
        else if (data.data[i].eventType == EdgeUpdateEventType)
        {
            std::string type = data.data[i].resourceType;
            if (type == CameraDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(camera_lock_);
                camera_instances[data.data[i].key]->updateInstance(data.data[i].data);
            }
            else if (type == LoudspeakerDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                loudspeaker_instances[data.data[i].key]->updateInstance(data.data[i].data);
            }
            else if (type == SensorDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(sensor_lock_);
                sensor_instances[data.data[i].key]->updateInstance(data.data[i].data);
            }
            else
            {
                LOG(ERROR) << data.data[i].key << "not supply the resourcetype";
            }
            // 更新数据库
            dbManager::getInstance().DBUpdateDeviceInstance(data.data[i].data);
        }
        else if (data.data[i].eventType == EdgeDeleteEventType)
        {
            std::string type = data.data[i].resourceType;
            if (type == CameraDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(camera_lock_);
                camera_instances.erase(data.data[i].key);
            }
            else if (type == LoudspeakerDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(loudspeaker_lock_);
                loudspeaker_instances.erase(data.data[i].key);
            }
            else if (type == SensorDeviceResourcetype)
            {
                std::lock_guard<std::mutex> lock(sensor_lock_);
                sensor_instances.erase(data.data[i].key);
            }
            else
            {
                LOG(ERROR) << data.data[i].key << "not supply the resourcetype";
            }
            // 删除数据库
            dbManager::getInstance().DBDelteDeviceInstance(data.data[i].key);
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
    thirdpack.packageType = SyncStepThree;
    thirdpack.hostname = device_hostname;
    std::string thirdmessage = MarshalMessageStruct(thirdpack);
    LOG(INFO) << "send message to cloud , type : " << thirdpack.packageType;
    if (!connection_->SendMessageToCloud(thirdmessage))
    {
        LOG(ERROR) << "send message fail";
    }
    LOG(INFO) << "This synchronization has been completed";
}

void Controller::HandleResourceSyncEdge(KeyAndDataPackages data)
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
}

void Controller::RefreshKVRecord()
{
    std::lock_guard<std::mutex> locker1(camera_lock_);
    std::lock_guard<std::mutex> locker2(loudspeaker_lock_);
    std::lock_guard<std::mutex> locker3(sensor_lock_);
    std::lock_guard<std::mutex> locker4(mic_lock_);
    this->versionRecord.clear();
    for (auto &iter : camera_instances)
    {
        this->versionRecord[iter.first] = iter.second->spec.version;
    }
    for (auto &iter : loudspeaker_instances)
    {
        this->versionRecord[iter.first] = iter.second->spec.version;
    }
    for (auto &iter : sensor_instances)
    {
        this->versionRecord[iter.first] = iter.second->spec.version;
    }
    for (auto &iter : mic_instances)
    {
        this->versionRecord[iter.first] = iter.second->spec.version;
    }
}

void Controller::PrintResource()
{
    std::lock_guard<std::mutex> locker1(camera_lock_);
    std::lock_guard<std::mutex> locker2(loudspeaker_lock_);
    std::lock_guard<std::mutex> locker3(sensor_lock_);
    std::lock_guard<std::mutex> locker4(mic_lock_);
    for (auto &iter : camera_instances)
    {
        std::cout << iter.second->Marshal() << std::endl;
    }
    for (auto &iter : loudspeaker_instances)
    {
        std::cout << iter.second->Marshal() << std::endl;
    }
    for (auto &iter : loudspeaker_instances)
    {
        std::cout << iter.second->Marshal() << std::endl;
    }
    for (auto &iter : mic_instances)
    {
        std::cout << iter.second->Marshal() << std::endl;
    }
}

std::string Controller::GetNonLocalFormat()
{
    KeyAndDataPackages data;
    data.hostname = device_hostname;
    data.packageType = EdgeSync;
    std::lock_guard<std::mutex> locker1(camera_lock_);
    std::lock_guard<std::mutex> locker2(loudspeaker_lock_);
    std::lock_guard<std::mutex> locker3(sensor_lock_);
    std::lock_guard<std::mutex> locker4(mic_lock_);
    for (auto &iter : camera_instances)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto res = std::make_shared<NonLocalResource>();
        int index = iter.first.find("/");
        res->name = iter.first.substr(index + 1);
        res->namespace_name = iter.first.substr(0, index);
        res->kind = iter.second->spec.kind;
        res->hostname = iter.second->spec.hostname;
        res->devicelist = iter.second->devicelist;
        pack.data = Marshal(res);
        data.data.emplace_back(pack);
    }
    for (auto &iter : sensor_instances)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto res = std::make_shared<NonLocalResource>();
        int index = iter.first.find("/");
        res->name = iter.first.substr(index + 1);
        res->namespace_name = iter.first.substr(0, index);
        res->kind = iter.second->spec.kind;
        res->hostname = iter.second->spec.hostname;
        res->devicelist = iter.second->devicelist;
        pack.data = Marshal(res);
        data.data.emplace_back(pack);
    }
    for (auto &iter : loudspeaker_instances)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto res = std::make_shared<NonLocalResource>();
        int index = iter.first.find("/");
        res->name = iter.first.substr(index + 1);
        res->namespace_name = iter.first.substr(0, index);
        res->kind = iter.second->spec.kind;
        res->hostname = iter.second->spec.hostname;
        res->devicelist = iter.second->devicelist;
        pack.data = Marshal(res);
        data.data.emplace_back(pack);
    }
    for (auto &iter : mic_instances)
    {
        KeyDatapack pack;
        pack.key = iter.first;
        pack.eventType = EdgeNonLocalEventType;
        auto res = std::make_shared<NonLocalResource>();
        int index = iter.first.find("/");
        res->name = iter.first.substr(index + 1);
        res->namespace_name = iter.first.substr(0, index);
        res->kind = iter.second->spec.kind;
        res->hostname = iter.second->spec.hostname;
        res->devicelist = iter.second->devicelist;
        pack.data = Marshal(res);
        data.data.emplace_back(pack);
    }
    return MarshalMessageStruct(data);
}

void Preprocessing()
{
    // read config yaml file
    std::cout << "load config from file";
    std::string config_file_path = "../config/controller_config.yaml";
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
    bool loadcrd = config["switch"]["loadcrd"].as<bool>();
    bool loadinstance = config["switch"]["loadinstance"].as<bool>();
    bool loadability = config["switch"]["loadability"].as<bool>();
    bool logintofile = config["switch"]["logintofile"].as<bool>();

    // config the log
    if (logintofile)
    {
        FLAGS_log_dir = LOG_FILE_PATH;
    }
    FLAGS_alsologtostderr = 1;
    google::InitGoogleLogging("abiltyframework-cpp");

    std::string httpserver = config["value"]["httpserveraddr"].as<std::string>();
    if (httpserver != "")
    {
        http_server_address = httpserver;
    }

    // get the local hostname
    char name[256];
    gethostname(name, sizeof(name));
    device_hostname = name;

    // load resource into database
    dbManager &manager = dbManager::getInstance();

    std::vector<std::string> crd_file_names;
    std::vector<std::string> instance_file_names;
    std::vector<std::string> ability_file_names;
    DIR *dir;
    struct dirent *ent;
    if (loadcrd && (dir = opendir(CRD_FILE_PATH)) != NULL)
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
        // need clear crd table first
        if (!manager.DBCleanCRD())
        {
            exit(0);
        }
        // add data into crd table
        for (int i = 0; i < crd_file_names.size(); i++)
        {
            std::string file_path = std::string(CRD_FILE_PATH) + "/" + crd_file_names[i];
            if (!manager.RegisterCrd(file_path))
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

    if (loadinstance && (dir = opendir(INSTANCE_FILE_PATH)) != NULL)
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
        if (!manager.DBCleanDeviceInstances())
        {
            exit(0);
        }
        for (int i = 0; i < instance_file_names.size(); i++)
        {
            std::string file_path = std::string(INSTANCE_FILE_PATH) + "/" + instance_file_names[i];
            if (!manager.AddDeviceInstance(file_path))
            {
                LOG(ERROR) << "add instance fail from file : " << file_path;
            }
        }
    }
    else
    {
        LOG(ERROR) << "Could not open directory : " << INSTANCE_FILE_PATH;
    }

    if (loadability && (dir = opendir(ABILITY_FILE_PATH)) != NULL)
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
        if (!manager.DBCleanAbility())
        {
            exit(0);
        }
        for (int i = 0; i < ability_file_names.size(); i++)
        {
            std::string file_path = std::string(ABILITY_FILE_PATH) + "/" + ability_file_names[i];
            if (!manager.AddAbilityInstance(file_path))
            {
                LOG(ERROR) << "add ability fail from file : " << file_path;
            }
        }
    }
    else
    {
        LOG(ERROR) << "Could not open directory : " << ABILITY_FILE_PATH;
    }

    // load cloud address form the database
    manager.DBGetCloudAddress();

    LOG(INFO) << "preprocessing success";
}