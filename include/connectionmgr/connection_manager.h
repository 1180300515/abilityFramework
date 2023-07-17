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
    /**
     * the end address record change callback
     */
    void OnEndAddressRecordChange(std::map<std::string, ConnectInfo> &address);
    /**
     * the cloud address record change callback
     */
    void OnCloudAddressRecordChange(const ConnectInfo &address);

    void Init(std::function<void(std::string)> servercall_);
    /**
     * @brief start the server to receive the message
     */
    void Run();
    bool ConnectWithEnd();
    bool ConnectWithCloud();
    bool DisconnectWithEnd();
    bool DisconnectWithCloud();
    void SendMessageToCloud(const std::string &data);
    void SendMessageToEnd(const std::string &data, std::optional<std::string> target_host);

private:
    std::unordered_map<std::string, ConnectInfo> endAddressRecord_; // Unique ID of the device（like hostname） and connect info
    std::mutex endlock_;
    ConnectInfo cloudAddressRecord_;
    std::mutex cloudlock_;

    std::shared_ptr<IConnection> CloudConnectionRecord; // Unique ID of the device（like hostname）
    std::unordered_map<std::string, std::shared_ptr<IConnection>> EndConnectionRecord;

    std::function<void(std::string)> recvCallback_;

    bool EndIsConnected = false;
    bool CloudIsConnected = false;

    /**
     * handle the connection status
     */
    void EndConnectionHandling();
    void CloudConnectionHandling();
};
#endif // CONNECTION_MANAGER_H