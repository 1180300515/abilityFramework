#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#include <string>

#include "global_var.h"
// extern the global var

// the cloud server address
extern std::string cloud_address;
// the http sever address
extern std::string http_server_address;
// the sync tcp client address
extern std::string edge_server_address;

extern std::string device_hostname;

// data package type
#define Notify "notify"
#define SyncStepOne "sync-kv"
#define SyncStepTwo "sync-s2c"
#define SyncStepThree "sync-c2s"
#define SyncAllOK "allok"
#define EdgeSync "edgesync"
// resource type
#define CameraDeviceResourcetype "CameraDevice"
#define SensorDeviceResourcetype "SensorDevice"
#define LoudspeakerDeviceResourcetype "LoudspeakerDevice"
#define MicrophoneDeviceResourcetype "MicrophoneDevice"
// event type
#define EdgeUpdateEventType "edgeupdate"
#define EdgeDeleteEventType "edgedelete"
#define EdgeNonLocalEventType "edgenonlocal"
#define EdgeAddEventType "edgeadd"
#define CloudUpdateEventType "cloudupdate"
#define CloudDelteEventType "clouddelete"
#define CloudAddEventType "cloudadd"
// default config
#define LOG_FILE_PATH "../log/"
#define DATABASE_FILE_PATH "../resource/db/ability.db"
#define CRD_FILE_PATH "../resource/resource_file/crd"
#define INSTANCE_FILE_PATH "../resource/resource_file/device"
#define ABILITY_FILE_PATH "../resource/resource_file/ability"


#endif