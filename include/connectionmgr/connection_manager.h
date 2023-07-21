#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <map>
#include <mutex>
#include <memory>
#include <functional>
#include <thread>

#include "connect_info.h"
#include "Iconnection.h"

class ConnectionManager
{
public:
    /**
     * called by discovery manager, when end address change
     */
    void OnEndAddressRecordChange(std::map<std::string, ConnectInfo> &address);
    /**
     * called by httpserver, when cloudaddress change
     */
    void OnCloudAddressRecordChange(const ConnectInfo &address);

    /**
     * @brief init the connection manager
     * @param servercall_
     */
    void Init(std::function<void(std::string)> servercall_);
    bool JudgeCloudAddressExist();
    bool JudgeEndAddressExist();
    /**
     * @brief connect with end,only exec once
     * @return
     */
    bool ConnectWithEnd();
    /**
     * @brief connect with cloud,only exec once
     * @return
     */
    bool ConnectWithCloud();
    /**
     * @brief disconnect with end
     * @return 
     */
    bool DisconnectWithEnd();
    bool DisconnectWithCloud();
    void SendMessageToCloud(const std::string &data);
    void SendMessageToEnd(const std::string &data, std::optional<std::string> target_host);

private:
    /**
     * handle the connection status
     */
    void EndConnectionHandling();
    void CloudConnectionHandling();

    std::unordered_map<std::string, ConnectInfo> endAddressRecord_; // Unique ID of the device（like hostname） and connect info
    std::mutex endlock_;
    ConnectInfo cloudAddressRecord_;
    std::mutex cloudlock_;

    std::shared_ptr<IConnection> CloudConnectionRecord; // Unique ID of the device（like hostname）
    std::unordered_map<std::string, std::shared_ptr<IConnection>> EndConnectionRecord;

    std::function<void(std::string)> recvCallback_;

    bool EndIsConnected = false;
    bool CloudIsConnected = false;

    std::map<std::string, std::thread> serverList; // type(udp,..)and the server thread,don't include the tcp,use host to instead
    std::map<std::string, std::shared_ptr<IConnection>> temporary_storage;//some protocol like udp need use a new object to start server
};
#endif // CONNECTION_MANAGER_H