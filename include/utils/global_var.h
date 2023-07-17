#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include <string>

#include "global_var.h"
// extern the global var

// the cloud server address
extern std::string cloud_address;
// the http sever address
extern std::string http_server_address;

extern std::string device_hostname;

// data package type
#define CloudNotify "notify"
#define CloudSyncStepOne "sync-kv"
#define CloudSyncStepTwo "sync-s2c"
#define CloudSyncStepThree "sync-c2s"
#define CloudSyncAllOK "allok"
#define EndSync "endsync"
#define EndSyncAllOK "endok"
// resource type
#define CameraDeviceResourcetype "CameraDevice"
#define SensorDeviceResourcetype "SensorDevice"
#define LoudspeakerDeviceResourcetype "LoudspeakerDevice"
#define MicrophoneDeviceResourcetype "MicrophoneDevice"
#define AbilityResourcetype "Ability"
// event type
#define EdgeUpdateEventType "edgeupdate"
#define EdgeDeleteEventType "edgedelete"
#define EdgeNonLocalEventType "edgenonlocal"
#define EdgeAddEventType "edgeadd"
#define CloudUpdateEventType "cloudupdate"
#define CloudDelteEventType "clouddelete"
#define CloudAddEventType "cloudadd"
// default config
#define LOG_FILE_PATH "log/"
#define DATABASE_FILE_PATH "resource/db/ability.db"
#define CRD_FILE_PATH "resource/resource_file/crd"
#define INSTANCE_FILE_PATH "resource/resource_file/device"
#define ABILITY_FILE_PATH "resource/resource_file/ability"
#define EDGE_SYNC_PORT 8001
//command type
#define CMD_START "start"
#define CMD_CONNECT "connect"
#define CMD_DISCONNECT "disconnect"
#define CMD_TERMINATE "terminate"
//life status
#define STATUS_INIT "INIT"
#define STATUS_STANDBY "STANDBY"
#define STATUS_RUNNING "RUNNING"
#define STATUS_SUSPEND "SUSPEND"
#define STATUS_TERMINATE "TERMINATE"

#endif