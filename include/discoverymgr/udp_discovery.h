#ifndef UDP_DISCOVERY_H
#define UDP_DISCOVERY_H

#include <string>
#include <functional>
#include <string>
#include <map>
#include <ctime>

struct Device
{
    std::string hostname;
    std::string ip;
    std::time_t last_online_time;
    std::time_t online_duration;
    std::string status;

    bool IsOffline(std::time_t current_time) const;
    void Update(const std::string &new_hostname, const std::string &new_status, std::time_t new_timestamp);
};

class UDPDiscovery
{
private:
    std::map<std::string, Device> devices;
    std::function<void(std::map<std::string, std::string>)> callback;

    std::string getHostName();
    void udp_broadcast_sender();
    void udp_broadcast_receiver();

public:
    void RegisterCallback(std::function<void(std::map<std::string, std::string>)> callback);
};

#endif // UDP_DISCOVERY_H