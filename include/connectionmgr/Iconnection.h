#ifndef _ICONNECTION_H
#define _ICONNECTION_H

#include <optional>
#include <string>
#include <functional>

class IConnection
{
public:
    virtual bool Connect(std::string address) = 0;
    virtual bool Disconnect() = 0;
    virtual bool SendMessage(const std::string &data) = 0;
    /**
     * @brief server to receive message
     * @param callback the receive message callback
     */
    virtual void StartServerToReceiveMessage(std::function<void(std::string)> callback) = 0;
    /**
     * @brief stop server
     */
    virtual void StopServerToReceiveMessage() = 0;
};

#endif // _ICONNECTION_H