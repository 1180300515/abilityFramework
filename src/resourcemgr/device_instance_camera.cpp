/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: device_instance_camera.cpp
 * Description: device_instance_camera.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/device_instance_camera.h"

#include <glog/logging.h>
#include <json/json.h>

#include <algorithm>

#include "resourcemgr/hardware_camera.h"

std::string CameraInstance::GetHardwareIdentifier() const { return spec.hardwareidentifier; }

bool CameraInstance::UpdateHardwareInfo(const Json::Value &info)
{
    // DLOG(INFO) << "camera hardware info \n" << info.toStyledString();
    bool changeornot = false;
    CameraHardware hardware;
    hardware.fromJson(info);
    if (hardware.bus_info == spec.hardwareidentifier) {
        if (spec.properties.card != hardware.card) {
            spec.properties.card = hardware.card;
            changeornot = true;
        }
        if (spec.properties.busInfo != hardware.bus_info) {
            spec.properties.busInfo = hardware.bus_info;
            changeornot = true;
        }
        if (hardware.device_path != spec.properties.devicePath) {
            spec.properties.devicePath = hardware.device_path;
            changeornot = true;
        }
        if (hardware.driver != spec.properties.driverName) {
            spec.properties.driverName = hardware.driver;
            changeornot = true;
        }
        if (hardware.formats.size() == spec.properties.supportFormat.size()) {
            for (const auto &iter : hardware.formats) {
                auto count = std::count(spec.properties.supportFormat.begin(), spec.properties.supportFormat.end(), iter);
                if (count == 0) {
                    spec.properties.supportFormat = hardware.formats;
                    changeornot = true;
                    break;
                }
            }
        }
    } else {
        DLOG(ERROR) << "this camera hardware bus_info: \"" << hardware.bus_info << "\" don't match this hardwareidentifier: \""
                    << spec.hardwareidentifier << "\"";
    }
    return changeornot;
}

void CameraInstance::EraseHardwareInfo()
{
    spec.properties.card = "";
    spec.properties.busInfo = "";
    spec.properties.devicePath = "";
    spec.properties.driverName = "";
    spec.properties.supportFormat.clear();
}

std::string CameraInstance::Marshal() const
{
    auto root = ToJson();
    Json::FastWriter writer;
    return writer.write(root);
}

Json::Value CameraInstance::ToJson() const
{
    Json::Value jnode;
    jnode["apiVersion"] = apiVersion;
    jnode["kind"] = kind;
    jnode["metadata"]["name"] = metadata.name;
    jnode["metadata"]["namespace"] = metadata.namespace_name;
    jnode["status"]["occupancy"] = status.occupancy;
    // spec part
    jnode["spec"]["kind"] = spec.kind;
    jnode["spec"]["hardwareidentifier"] = spec.hardwareidentifier;
    jnode["spec"]["version"] = spec.version;
    jnode["spec"]["hostname"] = spec.hostname;
    jnode["spec"]["properties"]["vendor"] = spec.properties.vendor;
    jnode["spec"]["properties"]["resolution"] = spec.properties.resolution;
    jnode["spec"]["properties"]["location"] = spec.properties.location;
    jnode["spec"]["properties"]["wideAngle"] = spec.properties.wideAngle;
    jnode["spec"]["properties"]["focusMethod"] = spec.properties.focusMethod;
    jnode["spec"]["properties"]["telephoto"] = spec.properties.telephoto;
    jnode["spec"]["properties"]["devicePath"] = spec.properties.devicePath;
    jnode["spec"]["properties"]["driverName"] = spec.properties.driverName;
    jnode["spec"]["properties"]["card"] = spec.properties.card;
    jnode["spec"]["properties"]["busInfo"] = spec.properties.busInfo;
    jnode["spec"]["properties"]["fov"] = spec.properties.fov;
    jnode["spec"]["properties"]["aspect"] = spec.properties.aspect;
    jnode["spec"]["properties"]["viewDistance"] = spec.properties.viewDistance;
    jnode["spec"]["properties"]["rotation"] = spec.properties.rotation;
    jnode["spec"]["properties"]["x"] = spec.properties.x;
    jnode["spec"]["properties"]["y"] = spec.properties.y;
    jnode["spec"]["properties"]["z"] = spec.properties.z;
    for (int i = 0; i < spec.properties.supportFormat.size(); i++) {
        jnode["spec"]["properties"]["supportFormat"].append(spec.properties.supportFormat[i]);
    }
    jnode["spec"]["properties"]["interface"] = spec.properties.interface;
    for (int i = 0; i < spec.capability1.size(); i++) {
        Json::Value cap;
        cap["name"] = spec.capability1[i].name;
        for (int j = 0; j < spec.capability1[i].api.size(); j++) {
            cap["api"].append(spec.capability1[i].api[j]);
        }
        jnode["spec"]["capability1"].append(cap);
    }
    for (int i = 0; i < spec.capability2.size(); i++) {
        Json::Value cap;
        cap["name"] = spec.capability2[i].name;
        for (int j = 0; j < spec.capability2[i].api.size(); j++) {
            cap["api"].append(spec.capability2[i].api[j]);
        }
        jnode["spec"]["capability2"].append(cap);
    }
    // for (auto &iter : spec.customprops)
    // {
    //     jnode["spec"]["customprops"][iter.first] = iter.second;
    // }
    // api part
    for (int i = 0; i < api.function.size(); i++) {
        Json::Value cap;
        cap["name"] = api.function[i].name;
        for (int j = 0; j < api.function[i].param.size(); j++) {
            Json::Value param;
            param["name"] = api.function[i].param[j].name;
            param["type"] = api.function[i].param[j].type;
            param["index"] = api.function[i].param[j].index;
            cap["param"].append(param);
        }
        for (int k = 0; k < api.function[i].returnparam.size(); k++) {
            Json::Value returnparam;
            returnparam["name"] = api.function[i].returnparam[k].name;
            returnparam["type"] = api.function[i].returnparam[k].type;
            returnparam["index"] = api.function[i].returnparam[k].index;
            cap["returnparam"].append(returnparam);
        }
        jnode["api"]["function"].append(cap);
    }
    // devicelist part
    for (int i = 0; i < devicelist.size(); i++) {
        Json::Value cap;
        cap["devicename"] = devicelist[i].devicename;
        cap["deviceid"] = devicelist[i].deviceid;
        cap["deviceip"] = devicelist[i].deviceip;
        cap["deviceport"] = devicelist[i].deviceport;
        cap["status"] = devicelist[i].status;
        jnode["devicelist"].append(cap);
    }
    return jnode;
}

bool CameraInstance::FromJson(const Json::Value &jnode)
{
    DeviceInstanceInfo::FromJson(jnode);
    spec.kind = jnode["spec"]["kind"].asString();
    spec.hardwareidentifier = jnode["spec"]["hardwareidentifier"].asString();
    spec.version = jnode["spec"]["version"].asString();
    spec.hostname = jnode["spec"]["hostname"].asString();

    if (spec.capability1.size() != 0) {
        spec.capability1.clear();
    }
    if (spec.capability2.size() != 0) {
        spec.capability2.clear();
    }
    if (spec.properties.supportFormat.size() != 0) {
        spec.properties.supportFormat.clear();
    }
    // if (spec.customprops.size() != 0)
    // {
    //     spec.customprops.clear();
    // }

    if (jnode["spec"].isMember("capability1")) {
        for (int i = 0; i < jnode["spec"]["capability1"].size(); i++) {
            Acapability cap;
            cap.name = jnode["spec"]["capability1"][i]["name"].asString();
            for (int j = 0; j < jnode["spec"]["capability1"][i]["api"].size(); j++) {
                cap.api.push_back(jnode["spec"]["capability1"][i]["api"][j].asString());
            }
            spec.capability1.emplace_back(cap);
        }
    }
    if (jnode["spec"].isMember("capability2")) {
        for (int i = 0; i < jnode["spec"]["capability2"].size(); i++) {
            Acapability cap;
            cap.name = jnode["spec"]["capability2"][i]["name"].asString();
            for (int j = 0; j < jnode["spec"]["capability2"][i]["api"].size(); j++) {
                cap.api.push_back(jnode["spec"]["capability2"][i]["api"][j].asString());
            }
            spec.capability2.emplace_back(cap);
        }
    }
    spec.properties.vendor = jnode["spec"]["properties"]["vendor"].asString();
    spec.properties.resolution = jnode["spec"]["properties"]["resolution"].asString();
    spec.properties.location = jnode["spec"]["properties"]["location"].asString();
    spec.properties.wideAngle = std::stoi(jnode["spec"]["properties"]["wideAngle"].asString());
    spec.properties.focusMethod = jnode["spec"]["properties"]["focusMethod"].asString();
    spec.properties.telephoto = (jnode["spec"]["properties"]["telephoto"].asString() == "true");
    spec.properties.devicePath = jnode["spec"]["properties"]["devicePath"].asString();
    spec.properties.driverName = jnode["spec"]["properties"]["driverName"].asString();
    spec.properties.card = jnode["spec"]["properties"]["card"].asString();
    spec.properties.busInfo = jnode["spec"]["properties"]["busInfo"].asString();
    spec.properties.fov = jnode["spec"]["properties"]["fov"].asString();
    spec.properties.aspect = jnode["spec"]["properties"]["aspect"].asString();
    spec.properties.viewDistance = jnode["spec"]["properties"]["viewDistance"].asString();
    spec.properties.rotation = jnode["spec"]["properties"]["rotation"].asString();
    spec.properties.x = jnode["spec"]["properties"]["x"].asString();
    spec.properties.y = jnode["spec"]["properties"]["y"].asString();
    spec.properties.z = jnode["spec"]["properties"]["z"].asString();
    for (int i = 0; i < jnode["spec"]["properties"]["supportFormat"].size(); i++) {
        spec.properties.supportFormat.emplace_back(jnode["spec"]["properties"]["supportFormat"][i].asString());
    }
    spec.properties.interface = jnode["spec"]["properties"]["interface"].asString();
    // if (jnode["spec"].isMember("customprops"))
    // {
    //     Json::Value::Members member;
    //     member = jnode["spec"]["customprops"].getMemberNames();
    //     for (Json::Value::Members::iterator it = member.begin(); it !=
    //     member.end(); it++)
    //     {
    //         spec.customprops[*it] =
    //         jnode["spec"]["customprops"][*it].asString();
    //     }
    // }
    return true;
}

bool CameraInstance::UnMarshal(const std::string &data)
{
    Json::Value jnode;
    Json::Reader reader;
    reader.parse(data, jnode);
    FromJson(jnode);
    return true;
}

bool CameraInstance::updateInstance(const Json::Value &jnode) { return FromJson(jnode); }

std::string CameraInstance::getInstanceVersion() const { return spec.version; }
