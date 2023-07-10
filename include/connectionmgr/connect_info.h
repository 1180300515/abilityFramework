#ifndef CONNECT_INFO_H
#define CONNECT_INFO_H

#include <string>

enum ProtocolTendency
{
    Random,
    SpeedFirst,
    LowPowerFirst
};

enum ProtocolType
{
    UDP,
    TCP,
    WebSocket,
    HTTP,
    BLE
};

// describe a connection
struct ConnectInfo
{
    std::string destinationAddress;
    ProtocolType protocoltype;
    ProtocolTendency tendency;
    bool isShortConnection;
    bool alreadyConnected;
};

#endif //CONNECT_INFO_H