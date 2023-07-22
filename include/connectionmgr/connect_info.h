#ifndef _CONNECT_INFO_H
#define _CONNECT_INFO_H

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
    Uninitialized,
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

#endif // _CONNECT_INFO_H