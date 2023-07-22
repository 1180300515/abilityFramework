#ifndef _CONNECT_BY_UDP_H
#define _CONNECT_BY_UDP_H

#include "Iconnection.h"

class ConnectByUDP : public IConnection
{
public:
    ConnectByUDP(std::string address);
    ~ConnectByUDP();
    bool Connect(std::string address);
    bool Disconnect();
    bool SendMessage(const std::string &data);
    void StartServerToReceiveMessage(std::function<void(std::string)> callback);
    void StopServerToReceiveMessage();

private:
    int sockid;
    std::string udpport;
    bool alive = true;

    std::string address; // the target address must include the port
};

#endif // _CONNECT_BY_UDP_H