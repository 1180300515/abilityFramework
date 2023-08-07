#include "hardware_device_resource_manager.h"

#include <httplib.h>

#include "glog/logging.h"

#include "global_var.h"
#include "resource_manager.h"

void HardwareResourceManager::Init(std::shared_ptr<ResourceManager> manager_, std::string hostname)
{
    this->resource_manager = manager_;
    this->hostname_ = hostname;
}

void HardwareResourceManager::EndAddressResult(std::map<std::string, std::string> result)
{
    compareOldAndNew(result);
    if (this->hardware_resources.size() == 0)
    {
        // the manager first start
        LOG(INFO) << "Insert all host device profile";
        for (const auto &iter : result)
        {
            if (iter.first == this->hostname_)
            {
                std::string data = this->resource_manager->GetHardwareDeviceInfo();
                DeviceProfile dp;
                Json::Value jnode;
                Json::Reader reader;
                reader.parse(data, jnode);
                dp.FromJson2Profile(jnode);
                hardware_resources[iter.first] = dp;
            }
            else
            {
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
            }
        }
        this->last_update = std::chrono::steady_clock::now();
        change = true;
    }
    else if (std::chrono::steady_clock::now() - this->last_update > std::chrono::minutes(5))
    {
        LOG(INFO) << "Update all host device profile";
        // beyond the time limit
        for (const auto &iter : result)
        {
            if (iter.first == this->hostname_)
            {
                std::string data = this->resource_manager->GetHardwareDeviceInfo();
                DeviceProfile dp;
                Json::Value jnode;
                Json::Reader reader;
                reader.parse(data, jnode);
                dp.FromJson2Profile(jnode);
                hardware_resources[iter.first] = dp;
            }
            else
            {
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
            }
        }
        this->last_update = std::chrono::steady_clock::now();
        change = true;
    }
    else
    {
        // only get the new host deviceprofile
        for (const auto &iter : result)
        {
            if (this->hardware_resources.count(iter.first) == 0)
            {
                LOG(INFO) << "new host: " << iter.first << " will get the host device profile";
                auto dp = getDeviceProfileFromHost(iter.second);
                hardware_resources[iter.first] = dp;
                change = true;
            }
        }
    }

    // regenerate records
    if (change)
    {
        std::lock_guard<std::mutex> locker(locker_);
        micDevices.clear();
        speakerDevices.clear();
        displayDevices.clear();
        cameraDevices.clear();
        for (const auto &item : hardware_resources)
        {
            for (const auto &micDevice : item.second.micDevices)
            {
                micDevices.emplace_back(micDevice.ToKeyValue(item.first));
            }
            for (const auto &speakerDevice : item.second.speakerDevices)
            {
                speakerDevices.emplace_back(speakerDevice.ToKeyValue(item.first));
            }
            for (const auto &cameraDevice : item.second.cameraDevices)
            {
                cameraDevices.emplace_back(cameraDevice.ToKeyValue(item.first));
            }
            for (const auto &displayDevice : item.second.displayDevices)
            {
                displayDevices.emplace_back(displayDevice.ToKeyValue(item.first));
            }
        }
        change = false;
    }
}

std::vector<std::string> HardwareResourceManager::GetHardwareResourceList(std::string type)
{
    std::lock_guard<std::mutex> lock(locker_);
    if (type == "mic")
    {
        return micDevices;
    }
    else if (type == "speaker")
    {
        return speakerDevices;
    }
    else if (type == "display")
    {
        return displayDevices;
    }
    else if (type == "camera")
    {
        return cameraDevices;
    }
    return std::vector<std::string>();
}

DeviceProfile HardwareResourceManager::getDeviceProfileFromHost(const std::string &ip)
{
    httplib::Client ccc(ip, HTTP_SERVER_PORT);
    DeviceProfile dp;
    auto res = ccc.Get("/api/Devices");

    if (res && res->status == 200)
    {
        // 响应成功，解析 JSON
        Json::CharReaderBuilder rbuilder;
        std::unique_ptr<Json::CharReader> const reader(rbuilder.newCharReader());
        Json::Value root;
        std::string errs;
        bool parsingSuccessful = reader->parse(res->body.c_str(), res->body.c_str() + res->body.size(), &root, &errs);
        if (!parsingSuccessful)
        {
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }
        // 反序列化 DeviceProfile
        dp.FromJson2Profile(root);
    }
    return dp;
}

void HardwareResourceManager::compareOldAndNew(std::map<std::string, std::string> new_)
{
    for (auto iter = hardware_resources.begin(); iter != hardware_resources.end();)
    {
        if (new_.count(iter->first) == 0)
        {
            LOG(INFO) << "host: " << iter->first << " is offline, the device profile will be deleted";
            hardware_resources.erase(iter++);
            change = true;
        }
        else
        {
            ++iter;
        }
    }
}