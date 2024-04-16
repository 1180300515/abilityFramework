/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_device_resource_manager.cpp
 * Description: hardware_device_resource_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/hardware_device_resource_manager.h"

#include <glog/logging.h>
#include <httplib.h>

#include "resourcemgr/resource_manager.h"
#include "utils/global_var.h"

void HardwareResourceManager::Init(std::shared_ptr<ResourceManager> manager_, std::string hostname)
{
    this->resource_manager = manager_;
    this->hostname_ = hostname;
}

void HardwareResourceManager::EndAddressResult(std::map<std::string, std::string> result)
{
    compareOldAndNew(result);
    if (this->hardware_resources.size() == 0) {
        // the manager first start
        DLOG(INFO) << "Insert all host device profile";
        for (const auto &iter : result) {
            if (iter.first == this->hostname_) {
                std::string data = this->resource_manager->GetHardwareDeviceInfo();
                //DLOG(INFO) << data ;
                DeviceProfile dp;
                Json::Value jnode;
                Json::Reader reader;
                reader.parse(data, jnode);
                jnode["ip"] = iter.second;
                //DLOG(INFO) << jnode.toStyledString();
                dp.FromJson2Profile(jnode);
                hardware_resources[iter.first] = dp;
            } else {
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
            }
        }
        this->last_update = std::chrono::steady_clock::now();
        change = true;
    } else if (std::chrono::steady_clock::now() - this->last_update > std::chrono::minutes(5)) {
        DLOG(INFO) << "Update all host device profile";
        // beyond the time limit
        for (const auto &iter : result) {
            if (iter.first == this->hostname_) {
                std::string data = this->resource_manager->GetHardwareDeviceInfo();
                DeviceProfile dp;
                Json::Value jnode;
                Json::Reader reader;
                reader.parse(data, jnode);
                jnode["ip"] = iter.second;
                dp.FromJson2Profile(jnode);
                hardware_resources[iter.first] = dp;
            } else {
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
            }
        }
        this->last_update = std::chrono::steady_clock::now();
        change = true;
    } else {
        // only get the new host deviceprofile
        for (const auto &iter : result) {
            if (this->hardware_resources.count(iter.first) == 0) {
                DLOG(INFO) << "new host: " << iter.first << " will get the host device profile";
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
                change = true;
            }
        }
    }

    // regenerate records
    if (change) {
        std::lock_guard<std::mutex> locker(locker_);
        micDevices.clear();
        speakerDevices.clear();
        displayDevices.clear();
        cameraDevices.clear();
        generalDevices.clear();
        for (const auto &item : hardware_resources) {
            for (const auto &micDevice : item.second.micDevices) {
                micDevices.emplace_back(micDevice.ToKeyValue(item.first));
            }
            for (const auto &speakerDevice : item.second.speakerDevices) {
                speakerDevices.emplace_back(speakerDevice.ToKeyValue(item.first));
            }
            for (const auto &cameraDevice : item.second.cameraDevices) {
                cameraDevices.emplace_back(cameraDevice.ToKeyValue(item.first, item.second.ip));
            }
            for (const auto &displayDevice : item.second.displayDevices) {
                displayDevices.emplace_back(displayDevice.ToKeyValue(item.first, item.second.ip));
            }
            for (const auto &device : item.second.generalDevices) {
                DLOG(INFO) << device.spec["kind"].asString();
                generalDevices[device.spec["kind"].asString()].emplace_back(device.spec.toStyledString());
                // DLOG(INFO) << device.ToJson().toStyledString();
            }
        }
        change = false;
    }
}

std::vector<std::string> HardwareResourceManager::GetHardwareResourceList(std::string type)
{
    std::lock_guard<std::mutex> lock(locker_);
    if (type == "mic") {
        return micDevices;
    } else if (type == "speaker") {
        return speakerDevices;
    } else if (type == "display") {
        return displayDevices;
    } else if (type == "camera") {
        std::vector<std::string> CameraDevices;
        // CameraDevices.insert(CameraDevices.end(), cameraDevices.begin(), cameraDevices.end());
        CameraDevices.insert(CameraDevices.end(), generalDevices["camera"].begin(), generalDevices["camera"].end());
        return CameraDevices;
    } else
    {
        return generalDevices[type];
    }
    
    return std::vector<std::string>();
}

DeviceProfile HardwareResourceManager::getDeviceProfileFromHost(const std::string &ip)
{
    httplib::Client ccc(ip, HTTP_SERVER_PORT);
    DeviceProfile dp;
    auto res = ccc.Get("/api/Devices");

    if (res && res->status == 200) {
        Json::CharReaderBuilder rbuilder;
        std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
        Json::Value root;
        std::string errs;
        bool parsingSuccessful = reader->parse(res->body.c_str(), res->body.c_str() + res->body.size(), &root, &errs);
        if (!parsingSuccessful) {
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }
        root["ip"] = ip;
        dp.FromJson2Profile(root);
    }
    return dp;
}

void HardwareResourceManager::compareOldAndNew(std::map<std::string, std::string> new_)
{
    for (auto iter = hardware_resources.begin(); iter != hardware_resources.end();) {
        if (new_.count(iter->first) == 0) {
            DLOG(INFO) << "host: " << iter->first << " is offline, the device profile will be deleted";
            hardware_resources.erase(iter++);
            change = true;
        } else {
            ++iter;
        }
    }
}
