#ifndef CONNECT_BY_BLE_H
#define CONNECT_BY_BLE_H

#include "Iconnection.h"

class ConnectByBLE : public IConnection
{
public:
    bool Connect(std::string address);
    bool Disconnect();
    std::string SendAndReceviceMessage(std::string data);

private:

};

#endif //CONNECT_BY_BLE_H