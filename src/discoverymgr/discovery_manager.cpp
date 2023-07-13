#include "discovery_manager.h"

#include "glog/logging.h"

void DiscoveryManager::ReceiveDeviceInfo(DiscoveryDeviceInfo info)
{
    std::lock_guard<std::mutex> locker(lock_);
    auto iter = devices.find(info.hostname);
    //insert ot update device
    if (iter == devices.end())
    {
        devices[info.hostname].emplace_back(info);
        LOG(INFO) << "new device : " << info.hostname;
    }
    else
    {
        for (auto &it : iter->second)
        {
            if (it.discovery_source == info.discovery_source)
            {
                it.Update(info.address, info.status, info.last_online_time);
            }
        }
    }
    // clean the timeout device
    for (auto it = devices.begin(); it != devices.end();)
    {
        for (auto in = it->second.begin(); in != it->second.end();)
        {
            if (in->IsOffline(std::time(nullptr)))
            {
                in = it->second.erase(in);
            }
            else
            {
                ++in;
            }
        }
        if (it->second.size() == 0)
        {
            it = devices.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
