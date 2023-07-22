#ifndef _CONNECT_BY_BLE_H
#define _CONNECT_BY_BLE_H

#include "Iconnection.h"

class ConnectByBLE : public IConnection
{
public:
    bool Connect(std::string address);
    bool Disconnect();
    bool SendMessage(const std::string &data);
    void StartServerToReceiveMessage(std::function<void(std::string)> callback);

private:
};

#endif // _CONNECT_BY_BLE_H