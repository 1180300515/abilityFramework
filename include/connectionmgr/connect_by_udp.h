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
    bool SendMessage(const std::string &data);
    void StartServerToReceiveMessage(std::function<void(std::string)> callback);

private:
    int sockid;
    std::string udpport;

    std::string address; //the target address must include the port
};

#endif //CONNECT_BY_UDP_H