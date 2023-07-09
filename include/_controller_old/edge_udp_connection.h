#ifndef EDGE_UDP_CONNECTION_H
#define EDGE_UDP_CONNECTION_H

#include "edge_connection.h"

class EdgeUDPConnection : public EdgeConnection
{
private:
    int sockId_;
public:
    EdgeUDPConnection(std::string address) : EdgeConnection(address){};
    ~EdgeUDPConnection();

    bool SendMessage(std::string data);
};

#endif // EDGE_UDP_CONNECTION_H