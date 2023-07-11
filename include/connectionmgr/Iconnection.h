#ifndef ICONNECTION_H
#define ICONNECTION_H

#include <optional>
#include <string>

class IConnection
{
public:
    virtual bool Connect(std::string address);
    virtual bool Disconnect();
    virtual std::optional<std::string> SendAndReceviceMessage(std::string data);
};

#endif // ICONNECTION_H