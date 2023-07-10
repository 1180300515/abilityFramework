#ifndef CONNECT_BY_UDP_H
#define CONNECT_BY_UDP_H

#include "Iconnection.h"

class ConnectbyUDP:public IConnection
{   
public:
    ConnectbyUDP(std::string address) : address(address){};
    bool Connect(std::string address);
    bool Disconnect();
    std::string SendAndReceviceMessage(std::string data);

private:
    int sockid_;
    std::string address;
};

#endif //CONNECT_BY_UDP_H