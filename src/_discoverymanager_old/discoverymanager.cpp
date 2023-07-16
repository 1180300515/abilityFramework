#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/discmgr_interface.h"
#include "discoverymanager/system_interface.h"
#include "discoverymanager/devpool_interface.h"

#include "plugincore/abimgr_interface.h"

#include <map>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "utils/color.h"

std::map<std::string, Device> devices;
// std::string hostname = "luo980-aorus15gyc";
DeviceProfile deviceProfile = getLocalHWInfo();
DevicePool devicePool = DevicePool(getHostName(), deviceProfile);
DevicePoolExtended devicePoolExtended(devicePool);
// extern DevicePool devicePool;
// extern DevicePoolExtended devicePoolExtended;

void udp_broadcast_sender(std::function<void(std::map<std::string, std::string>)> callback)
{

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    perror("socket");
    return;
  }

  int broadcast = 1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
  {
    perror("setsockopt");
    close(sock);
    return;
  }

  struct sockaddr_in addr
  {
  };
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_BROADCAST;

  while (1)
  {
    char hostname[40];
    gethostname(hostname, sizeof(hostname));
    std::string loadavg = get_system_status();
    std::string message = std::string(hostname) + "+" + loadavg + "+" + std::to_string(std::time(nullptr));

    std::cout << L_PURPLE << "Broadcasting: " << message << NONE << std::endl;

    if (sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      perror("sendto");
    }

    sleep(4);
    std::map<std::string, std::string> record;
    char name[256];
    gethostname(name, sizeof(name));
    for (auto &iter : devices)
    {
      // if (iter.second.status != statusToString[OVERLOADED] && iter.second.hostname != name)
      if (iter.second.hostname != name)
      {
        record[iter.second.hostname] = iter.second.ip + ":8001";
      }
    }
    callback(record);
  }

  close(sock);
}

void udp_broadcast_receiver()
{
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    perror("socket");
    return;
  }

  struct sockaddr_in addr
  {
  };
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8888);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    close(sock);
    return;
  }

  while (true)
  {
    char recv_buf[128];
    struct sockaddr_in remote_addr
    {
    };
    socklen_t addr_len = sizeof(remote_addr);

    ssize_t len = recvfrom(sock, recv_buf, sizeof(recv_buf) - 1, 0, (struct sockaddr *)&remote_addr, &addr_len);
    if (len < 0)
    {
      perror("recvfrom");
      break;
    }

    recv_buf[len] = '\0';
    std::string message(recv_buf);
    // std::cout << "Received: " << message << std::endl;
    std::istringstream iss(message);
    std::string hostname, status, timestamp_str;
    std::getline(iss, hostname, '+');
    std::getline(iss, status, '+');
    std::getline(iss, timestamp_str);
    std::time_t timestamp = std::stoll(timestamp_str);

    std::string ip = inet_ntoa(remote_addr.sin_addr);
    if (devices.find(ip) == devices.end())
    {
      // 增加设备对象
      std::cout << L_PURPLE << "New device: " << hostname << NONE << std::endl;
      devices[ip] = Device{hostname, ip, timestamp, 0, status};
      if (hostname != getHostName()){
        DeviceProfile dp = getDeviceProfileFromHost(ip);
        devicePool.addDevice(hostname, dp);
        devicePoolExtended.addDevice(hostname, dp);
      }
    }
    else
    {
      // 更新设备对象
      std::cout << L_PURPLE << "Update device: " << hostname << NONE << std::endl;
      devices[ip].Update(hostname, status, timestamp);
      // DeviceProfile dp = getDeviceProfileFromHost(ip);
      // devicePool.updateDevice(hostname, dp);
      // devicePoolExtended.updateDevice(hostname, dp);
    }

    // 检查并移除离线的设备
    auto current_time = std::time(nullptr);
    for (auto it = devices.begin(); it != devices.end();)
    {
      if (it->second.IsOffline(current_time))
      {
        std::cout << L_PURPLE << "Remove device: " << it->second.hostname << NONE << std::endl;
        devicePool.removeDevice(it->second.hostname);
        std::cout << PURPLE << "devicePool removed" << NONE << std::endl;
        devicePoolExtended.deleteDevice(it->second.hostname);
        std::cout << PURPLE << "devicePoolExtended removed" << NONE << std::endl;
        it = devices.erase(it);
        std::cout << PURPLE << "devices removed" << NONE << std::endl;
      }
      else
      {
        ++it;
      }
    }

    // Print devices list
    for (auto devices : devices)
    {
      std::cout << CYAN
                << devices.second.hostname
                << " "
                << devices.second.ip
                << " "
                << devices.second.status
                << " "
                << devices.second.online_duration
                << " "
                << devices.second.last_online_time
                << NONE << std::endl;
    }

    devicePoolExtended.print();
  }

  close(sock);
}

std::string getHostName()
{
  char hostname[256];
  if (gethostname(hostname, sizeof(hostname)) != 0)
  {
    // 获取主机名失败
    return "";
  }
  hostname[sizeof(hostname) - 1] = '\0'; // 确保以 null 结尾
  return std::string(hostname);
}