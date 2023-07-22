#ifndef _CONNECT_BY_HTTP_H
#define _CONNECT_BY_HTTP_H

#include "Iconnection.h"

class ConnectByHTTP : public IConnection
{
public:
    bool Connect(std::string address);
    bool Disconnect();
    bool SendMessage(const std::string &data);
    void StartServerToReceiveMessage(std::function<void(std::string)> callback);

private:
};

#endif // _CONNECT_BY_HTTP_H