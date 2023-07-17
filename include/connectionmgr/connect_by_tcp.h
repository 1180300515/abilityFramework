#ifndef CONNECT_BY_TCP_H
#define CONNECT_BY_TCP_H

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

private:
    const char *tcpaddr;
    uint16_t tcpport;
    int sockid = -1;
    std::string address; // the target address must include the port
};

#endif // CONNECT_BY_TCP_H