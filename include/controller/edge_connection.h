#ifndef EDGE_CONNECTION_H
#define EDGE_CONNECTION_H

#include <string>
#include <functional>

#include "message_package_struct.h"

class EdgeConnection {
public:
    std::string edgeaddress_;
public:
    EdgeConnection(std::string address) : edgeaddress_(address){};
    ~EdgeConnection() = default;
    /**
     * send message
     */
    virtual bool SendMessage(std::string data) = 0;
};


#endif //EDGE_CONNECTION_H