#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <sys/sysinfo.h>

enum SystemStatus
{
    IDLE,
    LIGHT_LOAD,
    MEDIUM_LOAD,
    HIGH_LOAD,
    OVERLOADED,
    UNKNOWN
};

std::map<SystemStatus, std::string> statusToString = 
{
    {IDLE, "Idle"},
    {LIGHT_LOAD, "Light Load"},
    {MEDIUM_LOAD, "Medium Load"},
    {HIGH_LOAD, "High Load"},
    {OVERLOADED, "Overloaded"},
    {UNKNOWN, "Unknown"}
};

std::string get_status_string(SystemStatus status)
{
    return statusToString[status];
}

SystemStatus get_system_status()
{
    std::ifstream loadavg_file("/proc/loadavg");
    if (!loadavg_file)
    {
        std::cerr << "Error: Unable to open /proc/loadavg" << std::endl;
        return UNKNOWN;
    }

    std::string loadavg_str;
    std::getline(loadavg_file, loadavg_str);
    std::istringstream iss(loadavg_str);

    double loadavg_1min, loadavg_5min, loadavg_15min;
    if (!(iss >> loadavg_1min >> loadavg_5min >> loadavg_15min))
    {
        std::cerr << "Error: Unable to read load averages" << std::endl;
        return UNKNOWN;
    }

    // 获取CPU核心数
    int num_cores = get_nprocs();

    double loadavg_1min_per_core = loadavg_1min / num_cores;
    double loadavg_5min_per_core = loadavg_5min / num_cores;
    double loadavg_15min_per_core = loadavg_15min / num_cores;

    // 根据负载判断系统状态
    if (loadavg_1min_per_core < 0.7)
    {
        return IDLE;
    }
    else if (loadavg_1min_per_core < 1)
    {
        return LIGHT_LOAD;
    }
    else if (loadavg_5min_per_core < 1)
    {
        return MEDIUM_LOAD;
    }
    else if (loadavg_15min_per_core < 1)
    {
        return HIGH_LOAD;
    }
    else
    {
        return OVERLOADED;
    }
}
