/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_camera.cpp
 * Description: hardware_camera.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/hardware_camera.h"

#include <iostream>

CameraHardware &CameraHardware::operator=(const CameraHardware &other)
{
    if (this != &other) {
        device_path = other.device_path;
        driver = other.driver;
        card = other.card;
        bus_info = other.bus_info;
        formats = other.formats;
        resolutions = other.resolutions;
    }
    return *this;
}

Json::Value CameraHardware::toJson() const
{
    Json::Value root;
    root["device_path"] = device_path;
    root["driver"] = driver;
    root["card"] = card;
    root["bus_info"] = bus_info;
    for (const auto &format : formats) {
        root["formats"].append(format);
    }
    for (const auto &resolution : resolutions) {
        //std::cout << resolution << std::endl;
        Json::Value subMsg;
        std::stringstream ss;
        ss << resolution;
        std::vector<std::string> res;
        std::string temp;
        while (getline(ss, temp, ','))
            res.push_back(temp);
        subMsg["width"] = std::stoi(res[0]);
        subMsg["height"] = std::stoi(res[1]);
        subMsg["fps"] = std::stoi(res[2]);
        root["resolutions"].append(subMsg);
    }
    //std::cout << root.toStyledString() << std::endl;
    return root;
}

void CameraHardware::fromJson(const Json::Value &root)
{
    device_path = root["device_path"].asString();
    driver = root["driver"].asString();
    card = root["card"].asString();
    bus_info = root["bus_info"].asString();
    for (const auto &format : root["formats"]) {
        formats.push_back(format.asString());
    }
    for (const auto &resolution : root["resolutions"]) {
        resolutions.push_back(resolution.toStyledString());
    }
}

std::string CameraHardware::ToKeyValue(std::string hostname, std::string ip) const
{
    Json::Value result;
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    result["hostname"] = hostname;
    ss << "ip" << ip << ",";
    result["ip"] = ip;
    ss << "device_path:" << device_path << ",";
    result["device_path"] = device_path;
    ss << "driver:" << driver << ",";
    result["driver"] = driver;
    ss << "card:" << card << ",";
    result["card"] = card;
    ss << "bus_info:" << bus_info << ",";
    result["bus_info"] = bus_info;
    ss << "formats:[";
    for (const auto &format : formats) {
        ss << format << " ";
        result["formats"].append(format);
    }
    ss << "],";
    ss << "resolutions:[";
    for (const auto &resolution : resolutions) {
        ss << resolution << " ";
        Json::Reader reader;
        Json::Value resolution_json;
        reader.parse(resolution,resolution_json);
        result["resolutions"].append(resolution_json);
    }
    ss << "]";
    return result.toStyledString();
}

void CameraHardware::print() const
{
    std::cout << "device_path: " << device_path << std::endl;
    std::cout << "driver: " << driver << std::endl;
    std::cout << "card: " << card << std::endl;
    std::cout << "bus_info: " << bus_info << std::endl;
    std::cout << "formats: ";
    for (const auto &format : formats) {
        std::cout << "  " << format << " ";
    }
    std::cout << std::endl << std::endl;
}
