/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: discovery_device_info.cpp
 * Description: discovery_device_info.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "discoverymgr/discovery_device_info.h"

bool DiscoveryDeviceInfo::IsOffline(std::time_t current_time) const
{
    return current_time - last_online_time > 30;  // 30 seconds
}

void DiscoveryDeviceInfo::Update(const std::string &new_address, const std::string &new_status, std::time_t new_timestamp)
{
    address = new_address;
    status = new_status;

    if (last_online_time != 0) {
        online_duration += new_timestamp - last_online_time;
    }

    last_online_time = new_timestamp;
}
