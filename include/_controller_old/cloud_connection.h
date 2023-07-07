#ifndef CLOUD_CONNECTION_H
#define CLOUD_CONNECTION_H

#include <string>
#include <functional>

#include "message_package_struct.h"

// abstract class
class CloudConnection
{
public:
    std::string cloudAddress_;
    std::function<void(KeyAndDataPackages)> callback_;

public:
    CloudConnection(std::string address) : cloudAddress_(address){};
    ~CloudConnection() = default;
    void RegisterCallback(std::function<void(KeyAndDataPackages)> func);
    bool CheckConnection();
    virtual void Connect() = 0;
    virtual void CloseConnection() = 0;
    virtual bool SendMessage(std::string data) = 0;
    virtual void ReceiveMessage() = 0;
};

#endif // CLOUD_CONNECTION_MANAGER_H