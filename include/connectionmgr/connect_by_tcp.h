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
    std::optional<std::string> SendAndReceviceMessage(std::string data);

private:
    const char *tcpaddr;
    uint16_t tcpport;
    int sockid = -1;
    std::string address; // the des address
};

#endif // CONNECT_BY_TCP_H