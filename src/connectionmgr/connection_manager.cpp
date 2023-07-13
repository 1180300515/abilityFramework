#include "connection_manager.h"

#include "glog/logging.h"

#include "connect_by_udp.h"
#include "connect_by_tcp.h"

void ConnectionManager::OnEndAddressRecordChange(std::map<std::string, ConnectInfo> &address)
{
    std::lock_guard<std::mutex> locker(endlock_);
    std::unordered_map<std::string, ConnectInfo> newrecord;

    for (const auto &iter : address)
    {
        if (endAddressRecord_.count(iter.first) == 0)
        {
            newrecord[iter.first] = iter.second;
        }
        else if (endAddressRecord_[iter.first].destinationAddress == iter.second.destinationAddress)
        {
            newrecord[iter.first] = endAddressRecord_[iter.first];
        }
        else
        {
            newrecord[iter.first] = iter.second;
        }
    }
    this->endAddressRecord_ = newrecord;
}

void ConnectionManager::OnCloudAddressRecordChange(std::map<std::string, ConnectInfo> &address)
{
    std::lock_guard<std::mutex> locker(cloudlock_);
    std::unordered_map<std::string, ConnectInfo> newrecord;
    for (const auto &iter : address)
    {
        if (cloudAddressRecord_.count(iter.first) == 0)
        {
            newrecord[iter.first] = iter.second;
        }
        else if (cloudAddressRecord_[iter.first].destinationAddress == iter.second.destinationAddress)
        {
            newrecord[iter.first] = cloudAddressRecord_[iter.first];
        }
        else
        {
            newrecord[iter.first] = iter.second;
        }
    }
    this->cloudAddressRecord_ = newrecord;
}

void ConnectionManager::Init(std::function<void(std::string)> cloud, std::function<void(std::string)> end)
{
    this->CloudCallback_ = cloud;
    this->EndCallback_ = end;
}

bool ConnectionManager::ConnectWithEnd()
{
    if (EndIsConnected)
    {
        LOG(INFO) << "please disconnect with end before connect";
        return false;
    }
    EndIsConnected = true;
    EndConnectionHandling();
    return true;
}

bool ConnectionManager::ConnectWithCloud()
{
    if (CloudIsConnected)
    {
        LOG(INFO) << "please disconnect with end before connect";
        return false;
    }
    CloudIsConnected = true;
    CloudConnectionHandling();
    return true;
}

bool ConnectionManager::DisconnectWithEnd()
{
    if (!EndIsConnected)
    {
        LOG(INFO) << "no need to disconnect";
        return false;
    }
    EndIsConnected = false;
    EndConnectionHandling();
    return true;
}

bool ConnectionManager::DisconnectWithCloud()
{
    if (!CloudIsConnected)
    {
        LOG(INFO) << "no need to disconnect";
        return false;
    }
    CloudIsConnected = false;
    CloudConnectionHandling();
    return false;
}

void ConnectionManager::SendAndReceiveMessageToCloud(const std::string &data)
{
    if (!CloudIsConnected)
    {
        LOG(ERROR) << "please connect before send message";
    }
    else
    {
        for (const auto &iter : CloudConnectionRecord)
        {
            auto iterdata = iter.second->SendAndReceviceMessage(data);
            if (iterdata != std::nullopt)
            {
                this->CloudCallback_(iterdata.value());
            }
            else
            {
                LOG(WARNING) << "send message to :" << iter.first << " fail";
            }
        }

        LOG(INFO) << "send and receive message to cloud success";
    }
}

void ConnectionManager::SendAndReceiveMessageToEnd(const std::string &data)
{
    if (!EndIsConnected)
    {
        LOG(ERROR) << "please connect before send message";
    }
    else
    {
        for (const auto &iter : EndConnectionRecord)
        {
            auto iterdata = iter.second->SendAndReceviceMessage(data);
            if (iterdata != std::nullopt)
            {
                this->EndCallback_(iterdata.value());
            }
            else
            {
                LOG(WARNING) << "send message to :" << iter.first << " fail";
            }
        }
        LOG(INFO) << "send and receive message to end success";
    }
}

void ConnectionManager::EndConnectionHandling()
{
    std::lock_guard<std::mutex> locker1(endlock_);
    // clean the nonexist connection
    for (auto it = EndConnectionRecord.begin(); it != EndConnectionRecord.end();)
    {
        if (endAddressRecord_.count(it->first) == 0)
        {
            it->second->Disconnect();
            EndConnectionRecord.erase(it++);
        }
        else
        {
            it++;
        }
    }

    // None status handle
    for (auto &iter : endAddressRecord_)
    {
        if (iter.second.status == ConnectionStatus::None)
        {
            if (iter.second.protocoltype == ProtocolType::RandomProtocol)
            {
                if (iter.second.tendency == ProtocolTendency::Random)
                {
                    auto connect = std::make_shared<ConnectByUDP>(iter.first);
                    this->EndConnectionRecord[iter.first] = connect;
                }
            }
            else if (iter.second.protocoltype == ProtocolType::UDP)
            {
                auto connect = std::make_shared<ConnectByUDP>(iter.first);
                this->EndConnectionRecord[iter.first] = connect;
            }
            else if (iter.second.protocoltype == ProtocolType::TCP)
            {
                auto connect = std::make_shared<ConnectByTCP>(iter.first);
                this->EndConnectionRecord[iter.first] = connect;
            }
            iter.second.status = ConnectionStatus::Disconnected;
        }
    }

    // connected or disconnected status check
    for (auto &iter : endAddressRecord_)
    {
        if (EndIsConnected)
        {
            if (this->EndConnectionRecord[iter.first]->Connect(iter.second.destinationAddress))
            {
                iter.second.status = ConnectionStatus::Connected;
            }
            else
            {
                LOG(ERROR) << "connect with end : " << iter.first << " fail";
            }
        }
        else
        {
            if (this->EndConnectionRecord[iter.first]->Disconnect())
            {
                iter.second.status = ConnectionStatus::Disconnected;
            }
            else
            {
                LOG(ERROR) << "disconnect with end : " << iter.first << " fail";
            }
        }
    }
}

void ConnectionManager::CloudConnectionHandling()
{
    std::lock_guard<std::mutex> locker2(cloudlock_);

    // clean the nonexist connection
    for (auto it = CloudConnectionRecord.begin(); it != CloudConnectionRecord.end();)
    {
        if (cloudAddressRecord_.count(it->first) == 0)
        {
            it->second->Disconnect();
            CloudConnectionRecord.erase(it++);
        }
        else
        {
            it++;
        }
    }

    // None status handle
    for (auto &iter : cloudAddressRecord_)
    {
        if (iter.second.status == ConnectionStatus::None)
        {
            if (iter.second.protocoltype == ProtocolType::RandomProtocol)
            {
                if (iter.second.tendency == ProtocolTendency::Random)
                {
                    auto connect = std::make_shared<ConnectByUDP>(iter.first);
                    this->EndConnectionRecord[iter.first] = connect;
                }
            }
            else if (iter.second.protocoltype == ProtocolType::UDP)
            {
                auto connect = std::make_shared<ConnectByUDP>(iter.first);
                this->EndConnectionRecord[iter.first] = connect;
            }
            else if (iter.second.protocoltype == ProtocolType::TCP)
            {
                auto connect = std::make_shared<ConnectByTCP>(iter.first);
                this->EndConnectionRecord[iter.first] = connect;
            }
            iter.second.status = ConnectionStatus::Disconnected;
        }
    }

    // connected or disconnected status check
    for (auto &iter : cloudAddressRecord_)
    {
        if (CloudIsConnected)
        {
            if (this->CloudConnectionRecord[iter.first]->Connect(iter.second.destinationAddress))
            {
                iter.second.status = ConnectionStatus::Connected;
            }
            else
            {
                LOG(ERROR) << "connect with cloud : " << iter.first << " fail";
            }
        }
        else
        {
            if (this->CloudConnectionRecord[iter.first]->Disconnect())
            {
                iter.second.status = ConnectionStatus::Disconnected;
            }
            else
            {
                LOG(ERROR) << "disconnect with cloud : " << iter.first << " fail";
            }
        }
    }
}
