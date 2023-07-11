#ifndef CONNECT_BY_UDP_H
#define CONNECT_BY_UDP_H

#include "Iconnection.h"

class ConnectByUDP:public IConnection
{   
public:
    ConnectByUDP(std::string address) : address(address){};
    ~ConnectByUDP();
    bool Connect(std::string address);
    bool Disconnect();
    std::optional<std::string> SendAndReceviceMessage(std::string data);

private:
    int sockid;
    std::string address;
};

#endif //CONNECT_BY_UDP_H