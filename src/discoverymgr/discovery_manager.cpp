/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: discovery_manager.cpp
 * Description: discovery_manager.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "discoverymgr/discovery_manager.h"

#include <glog/logging.h>

#include "utils/color.h"

void DiscoveryManager::ReceiveDeviceInfo(DiscoveryDeviceInfo info)
{
    std::lock_guard<std::mutex> locker(lock_);
    auto iter = devices.find(info.hostname);
    // insert ot update device
    if (iter == devices.end()) {
        devices[info.hostname].emplace_back(info);
        DLOG(INFO) << L_BLUE << "new device : " << info.hostname << NONE;
    } else {
        for (auto &it : iter->second) {
            if (it.discovery_source == info.discovery_source) {
                it.Update(info.address, info.status, info.last_online_time);
            }
        }
    }
    // clean the timeout device
    for (auto it = devices.begin(); it != devices.end();) {
        for (auto in = it->second.begin(); in != it->second.end();) {
            if (in->IsOffline(std::time(nullptr))) {
                DLOG(INFO) << "host : " << it->first << ", the " << in->discovery_source << " is offline";
                in = it->second.erase(in);
            } else {
                ++in;
            }
        }
        if (it->second.size() == 0) {
            DLOG(INFO) << "host: " << it->first << " is offline";
            it = devices.erase(it);
        } else {
            ++it;
        }
    }
}

void DiscoveryManager::Init(std::function<void(std::map<std::string, std::string>)> resource_)
{
    DLOG(INFO) << L_GREEN << "init discovery manager" << NONE;
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        DLOG(ERROR) << RED << "get hostname fail" << NONE;
    }
    hostname[sizeof(hostname) - 1] = '\0';
    this->hostname_ = std::string(hostname);

    resource_callback = resource_;

    this->lanipv4discovery_ = std::make_shared<LANIPV4Discovery>();
    this->lanipv4discovery_->RegisterCallback(std::bind(&DiscoveryManager::ReceiveDeviceInfo, this, std::placeholders::_1));
}

void DiscoveryManager::Run()
{
    // start receiver
    this->lanipv4discovery_->RunBroadcastReceiver();

    this->discovery_thread = std::thread([this]() {
        while (1) {
            this->lanipv4discovery_->BroadcastSender();

            sleep(4);
            // deal the device and send to callbak func
            std::map<std::string, std::string> resource_callback_info;
            for (const auto &iter : devices) {
                DLOG(INFO) << "Discovery result : the device :" << iter.first << " address: " << iter.second.front().address;
                resource_callback_info[iter.first] = iter.second.front().address;
            }
            // tell the resource manager about the discovery result
            resource_callback(resource_callback_info);
        }
    });
}
