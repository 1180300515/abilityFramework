#ifndef _SYSTEM_INTERFACE_H
#define _SYSTEM_INTERFACE_H

#include <string>

enum class SystemLoad
{
    Increasing,
    Decreasing,
    Stable,
    Unknown
};

enum class SystemStatus
{
    Overloaded,
    Busy,
    Optimal,
    Idle,
    Unknown
};

std::string systemStatusToString(SystemStatus status);

std::string systemLoadToString(SystemLoad load);

SystemLoad getSystemLoad(double load1, double load5, double load15);

SystemStatus getSystemStatus(double load1, double load15, int cpuCores);

std::string get_system_status();

#endif // _SYSTEM_INTERFACE_H