#ifndef CONNECT_BY_HTTP_H
#define CONNECT_BY_HTTP_H

#include "Iconnection.h"

class ConnectByHTTP : public IConnection
{
public:
    bool Connect(std::string address);
    bool Disconnect();
    std::string SendAndReceviceMessage(std::string data);

private:

};

#endif //CONNECT_BY_HTTP_H