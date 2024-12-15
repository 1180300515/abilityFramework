#ifndef LIFECYCLEMGR_HEARTBEAT_INFO_H_
#define LIFECYCLEMGR_HEARTBEAT_INFO_H_

#include <json/json.h>

#include <chrono>  // NOLINT [build/c++11]
#include <string>

struct HeartbeatInfo {
    std::string abilityName;  // 能力名称
    int IPCPort = -1;              // 能力控制端口
    int abilityPort = -1;          // 能力服务端口
    std::string state;       // 当前状态
    std::chrono::steady_clock::time_point last_update;

    [[nodiscard]] Json::Value toJson() const;
    void parseJson(const Json::Value &root);
    bool IsOffline() const;
};

#endif  // LIFECYCLEMGR_HEARTBEAT_INFO_H_
