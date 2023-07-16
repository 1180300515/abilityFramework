#ifndef CONNECT_BY_WEBSOCKET_H
#define CONNECT_BY_WEBSOCKET_H

#include "Iconnection.h"

class ConnectByWebsocket : public IConnection
{
public:
    bool Connect(std::string address);
    bool Disconnect();
    std::optional<std::string> SendAndReceviceMessage(std::string data);

private:
};

#endif //CONNECT_BY_WEBSOCKET_H