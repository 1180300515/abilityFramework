#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <map>
#include <mutex>
#include <memory>
#include <functional>

#include "connect_info.h"
#include "Iconnection.h"

class ConnectionManager
{
public:
    void OnEndAddressRecordChange(std::map<std::string, ConnectInfo> &address);
    void OnCloudAddressRecordChange(std::map<std::string, ConnectInfo> &address);

    void Init(std::function<void(std::string)> cloud, std::function<void(std::string)> end);
    bool ConnectWithEnd();
    bool ConnectWithCloud();
    bool DisconnectWithEnd();
    bool DisconnectWithCloud();
    void SendAndReceiveMessageToCloud(const std::string &data);
    void SendAndReceiveMessageToEnd(const std::string &data);

    
private:
    std::map<std::string, ConnectInfo> endAddressRecord_;
    std::mutex endlock_;
    std::map<std::string, ConnectInfo> cloudAddressRecord_;
    std::mutex cloudlock_;

    std::map<std::string, std::shared_ptr<IConnection>> CloudConnectionRecord;
    std::map<std::string, std::shared_ptr<IConnection>> EndConnectionRecord;

    std::function<void(std::string)> CloudCallback_;
    std::function<void(std::string)> EndCallback_;



};
#endif // CONNECTION_MANAGER_H