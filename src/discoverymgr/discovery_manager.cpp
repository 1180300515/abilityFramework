#include "discovery_manager.h"

#include "glog/logging.h"

void DiscoveryManager::ReceiveDeviceInfo(DiscoveryDeviceInfo info)
{
    std::lock_guard<std::mutex> locker(lock_);
    auto iter = devices.find(info.hostname);
    // insert ot update device
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

void DiscoveryManager::Init(std::function<void(std::map<std::string, ConnectInfo>)> call)
{
    callback = call;
    this->lanipv4discovery_ = std::make_shared<LANIPV4Discovery>();
    this->lanipv4discovery_->RegisterCallback(std::bind(&DiscoveryManager::ReceiveDeviceInfo, this, std::placeholders::_1));

    this->blediscovery_ = std::make_shared<BLEDiscovery>();
    
}

void DiscoveryManager::Run()
{
    //start receiver
    this->lanipv4discovery_->RunBroadcastReceiver();

    while(1)
    {
        this->lanipv4discovery_->BroadcastSender();

        sleep(4);
        //deal the device and send to callbak func
        std::map<std::string, ConnectInfo> callback_info;
        for (const auto &iter : devices)
        {
            ConnectInfo new_info;
            new_info.destinationAddress = iter.second.front().address;
            new_info.protocoltype = ProtocolType::RandomProtocol;
            new_info.status = ConnectionStatus::None;
            new_info.tendency = ProtocolTendency::Random;
            callback_info[iter.first] = new_info;
        }
        callback(callback_info);
    }

}
