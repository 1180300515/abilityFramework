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
    RandomProtocol,
    UDP,
    TCP,
    WebSocket,
    HTTP,
    BLE
};

enum ConnectionStatus
{
    None,
    Connected,
    Disconnected
};

// describe a connection
struct ConnectInfo
{
    std::string destinationAddress; // this address don't include the port(if ipv4)
    ProtocolType protocoltype;
    ProtocolTendency tendency;

    // describe the connect status
   ConnectionStatus status;
};

#endif //CONNECT_INFO_H