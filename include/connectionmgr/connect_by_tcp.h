#ifndef _CONNECT_BY_TCP_H
#define _CONNECT_BY_TCP_H

#include "Iconnection.h"

class ConnectByTCP : public IConnection
{
public:
    ConnectByTCP(std::string address) : address(address){};
    ~ConnectByTCP();
    bool Connect(std::string address);
    bool Disconnect();
    bool SendMessage(const std::string &data);
    void StartServerToReceiveMessage(std::function<void(std::string)> callback);
    void StopServerToReceiveMessage();

private:
    const char *tcpaddr;
    uint16_t tcpport;
    bool alive = true;

    int sockid = -1;
    std::string address; // the target address must include the port
};

#endif // _CONNECT_BY_TCP_H