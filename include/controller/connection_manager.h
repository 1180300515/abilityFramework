#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <string>
#include <map>
#include <functional>
#include <memory>

#include "message_package_struct.h"
#include "cloud_connection.h"
#include "edge_connection.h"

// declare the class Controller
class Controller;

class ConnectionManager
{
private:
    // record the non local edge hostname and address
    std::map<std::string, std::string> edgeAddressRecord_;
    std::mutex lock_;
    std::shared_ptr<CloudConnection> cloud_;
    std::function<void(KeyAndDataPackages)> cloudCallback_;
    std::function<void(KeyAndDataPackages)> edgeCallback_;

    std::shared_ptr<Controller> controller_;

public:
    ConnectionManager(std::shared_ptr<Controller> controller);
    ~ConnectionManager() = default;

    /**
     * register the cloud callback to handle the data from the cloud
    */
    void RegisterCloudCallback(std::function<void(KeyAndDataPackages)> func);
    /**
     * register edge callback to handle the data from other edge
    */
    void RegisterEdgeCallback(std::function<void(KeyAndDataPackages)> func);
    /**
     * send message to cloud
    */
    bool SendMessageToCloud(std::string data);
    /**
     * set edge address record
    */
    void SetEdgeAddressRecord(std::map<std::string , std::string> record);
    /**
     * start the http server
    */
    void HttpServerThread();
    /**
     * start the sync with the cloud
    */
    void CloudSyncThread();
    /**
     * start the server to receive data from other edge
    */
    void EdgeSyncServerThread();
    /**
     * start the sync with the edge
    */
    void EdgeSyncThread();
    /**
     * start the connection manager
    */
    void Run();
};

#endif // CONNECTION_MANAGER_H