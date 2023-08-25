/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: global_var.h
 * Description: global_var.h
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#ifndef UTILS_GLOBAL_VAR_H_
#define UTILS_GLOBAL_VAR_H_

#include <cstdlib>
#include <string>

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
#define EDGE_SYNC_PORT 8001  // include tcp and udp
#define HTTP_SERVER_PORT 8080
// command type
#define CMD_START "start"
#define CMD_CONNECT "connect"
#define CMD_DISCONNECT "disconnect"
#define CMD_TERMINATE "terminate"
// life status
#define STATUS_INIT "INIT"
#define STATUS_STANDBY "STANDBY"
#define STATUS_RUNNING "RUNNING"
#define STATUS_SUSPEND "SUSPEND"
#define STATUS_TERMINATE "TERMINATE"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof *(a))

#endif  // UTILS_GLOBAL_VAR_H_
