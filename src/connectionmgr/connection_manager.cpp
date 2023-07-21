#include "connection_manager.h"

#include "glog/logging.h"

#include "connect_by_udp.h"
#include "connect_by_tcp.h"

void ConnectionManager::OnEndAddressRecordChange(std::map<std::string, ConnectInfo> &address)
{
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
    EndConnectionHandling();
}

void ConnectionManager::OnCloudAddressRecordChange(const ConnectInfo &address)
{
    {
        std::lock_guard<std::mutex> locker(cloudlock_);
        if (cloudAddressRecord_.destinationAddress == address.destinationAddress)
        {
            cloudAddressRecord_ = address;
        }
        else
        {
            cloudAddressRecord_ = address;
        }
    }
    CloudConnectionHandling();
}

void ConnectionManager::Init(std::function<void(std::string)> servercall_)
{
    this->recvCallback_ = servercall_;
}

bool ConnectionManager::JudgeCloudAddressExist()
{
    if (cloudAddressRecord_.destinationAddress == "")
    {
        return false;
    }
    else
    {
        return true;
    }
    return false;
}

bool ConnectionManager::JudgeEndAddressExist()
{
    if (endAddressRecord_.size() == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
    return false;
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
    return true;
}

void ConnectionManager::SendMessageToCloud(const std::string &data)
{
    if (!CloudIsConnected)
    {
        LOG(ERROR) << "please connect before send message";
    }
    else
    {
        if (CloudConnectionRecord->SendMessage(data))
        {
            LOG(INFO) << "send and receive message to cloud success";
        }
        else
        {
            LOG(WARNING) << "send message to :" << cloudAddressRecord_.destinationAddress << " fail";
        }
    }
}

void ConnectionManager::SendMessageToEnd(const std::string &data, std::optional<std::string> target_host)
{
    if (!EndIsConnected)
    {
        LOG(ERROR) << "please connect before send message";
    }
    else
    {
        if (target_host == std::nullopt)
        {
            for (const auto &iter : EndConnectionRecord)
            {
                if (iter.second->SendMessage(data))
                {
                    LOG(INFO) << "send message to end : " << iter.first << " success";
                }
                else
                {
                    LOG(WARNING) << "send message to :" << iter.first << " fail";
                }
            }
        }
        else
        {
            if (EndConnectionRecord[target_host.value()]->SendMessage(data))
            {
                LOG(INFO) << "send message to end : " << target_host.value() << " success";
            }
            else
            {
                LOG(WARNING) << "send message to :" << target_host.value() << " fail";
            }
        }
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
            it->second->StopServerToReceiveMessage();
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
    // server start
    if (EndIsConnected)
    {
        for (const auto &iter : endAddressRecord_)
        {
            if (iter.second.protocoltype == ProtocolType::TCP && iter.second.status == ConnectionStatus::Connected)
            {
                serverList[iter.first] = std::thread(std::bind(&IConnection::StartServerToReceiveMessage, this->EndConnectionRecord[iter.first], std::placeholders::_1), recvCallback_);
                continue;
            }
            if (iter.second.protocoltype == ProtocolType::UDP && iter.second.status == ConnectionStatus::Connected)
            {
                if (serverList.count("udp") == 0)
                {
                    auto new_object = std::make_shared<ConnectByUDP>(iter.first);
                    temporary_storage["udp"] = new_object;
                    serverList[iter.first] = std::thread(std::bind(&IConnection::StartServerToReceiveMessage, new_object, std::placeholders::_1), recvCallback_);
                }
                continue;
            }
            // other protocol write in there
        }
    }
    // server stop
    if (!EndIsConnected)
    {
        for (const auto &iter : endAddressRecord_)
        {
            if (iter.second.protocoltype == ProtocolType::TCP && iter.second.status == ConnectionStatus::Disconnected)
            {
                this->EndConnectionRecord[iter.first]->StopServerToReceiveMessage();
                this->serverList[iter.first].join(); // wait the thread to end
                this->serverList.erase(iter.first);
                continue;
            }
            if (iter.second.protocoltype == ProtocolType::UDP && iter.second.status == ConnectionStatus::Disconnected)
            {
                temporary_storage["udp"]->StopServerToReceiveMessage();
                this->serverList["udp"].join();
                this->serverList.erase(iter.first);
            }
        }
    }
}

void ConnectionManager::CloudConnectionHandling()
{
    std::lock_guard<std::mutex> locker2(cloudlock_);
    if (cloudAddressRecord_.destinationAddress == "")
    {
        LOG(INFO) << "cloud address is not exist";
        return;
    }

    // None status handle
    if (cloudAddressRecord_.status == ConnectionStatus::None)
    {
        if (cloudAddressRecord_.protocoltype == ProtocolType::RandomProtocol)
        {
            if (cloudAddressRecord_.tendency == ProtocolTendency::Random)
            {
                this->CloudConnectionRecord = std::make_shared<ConnectByUDP>(cloudAddressRecord_.destinationAddress);
            }
        }
        else if (cloudAddressRecord_.protocoltype == ProtocolType::UDP)
        {
            this->CloudConnectionRecord = std::make_shared<ConnectByUDP>(cloudAddressRecord_.destinationAddress);
        }
        else if (cloudAddressRecord_.protocoltype == ProtocolType::TCP)
        {
            this->CloudConnectionRecord = std::make_shared<ConnectByTCP>(cloudAddressRecord_.destinationAddress);
        }
        cloudAddressRecord_.status = ConnectionStatus::Disconnected;
    }

    // connected or disconnected status check
    if (CloudIsConnected)
    {
        if (this->CloudConnectionRecord->Connect(cloudAddressRecord_.destinationAddress))
        {
            cloudAddressRecord_.status = ConnectionStatus::Connected;
        }
        else
        {
            LOG(ERROR) << "connect with cloud : " << cloudAddressRecord_.destinationAddress << " fail";
        }
    }
    else
    {
        if (this->CloudConnectionRecord->Disconnect())
        {
            cloudAddressRecord_.status = ConnectionStatus::Disconnected;
        }
        else
        {
            LOG(ERROR) << "disconnect with cloud : " << cloudAddressRecord_.destinationAddress << " fail";
        }
    }
}
