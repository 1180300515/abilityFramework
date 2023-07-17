#ifndef CONNECT_BY_BLE_H
#define CONNECT_BY_BLE_H

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

#endif //CONNECT_BY_BLE_H