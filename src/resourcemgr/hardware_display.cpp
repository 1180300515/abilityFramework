/*
 * Copyright (C), 2022-2023, Soochow University & OPPO Mobile Comm Corp., Ltd.
 *
 * File: hardware_display.cpp
 * Description: hardware_display.cpp
 * Version: V1.0.0
 * Date: 2023/08/24
 * Author: Soochow University
 * Revision History:
 *   Version       Date          Author         Revision Description
 *  V1.0.0        2023/08/24    Soochow University       Create and initialize
 */

#include "resourcemgr/hardware_display.h"

#include <iostream>

Json::Value DisplayHardware::toJson() const
{
    Json::Value root;
    root["screen"] = screen;
    root["x"] = x;
    root["y"] = y;
    root["width"] = width;
    root["height"] = height;
    root["mmWidth"] = mmWidth;
    root["mmHeight"] = mmHeight;
    return root;
}

void DisplayHardware::fromJson(const Json::Value &root)
{
    screen = root["screen"].asInt();
    x = root["x"].asInt();
    y = root["y"].asInt();
    width = root["width"].asInt();
    height = root["height"].asInt();
    mmWidth = root["mmWidth"].asInt();
    mmHeight = root["mmHeight"].asInt();
}

std::string DisplayHardware::ToKeyValue(std::string hostname, std::string ip) const
{
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "ip" << ip << ",";
    ss << "screen:" << screen << ",";
    ss << "x:" << x << ",";
    ss << "y:" << y << ",";
    ss << "width:" << width << ",";
    ss << "height:" << height << ",";
    ss << "mmWidth:" << mmWidth << ",";
    ss << "mmHeight:" << mmHeight;
    Json::Value device;
    device["hostname"] = hostname;
    device["ip"] = ip;
    device["screen"] = screen;
    device["x"] = x;
    device["y"] = y;
    device["width"] = width;
    device["height"] = height;
    device["mmWidth"] = mmWidth;
    device["mmHeight"] = mmHeight;
    return device.toStyledString();
}

void DisplayHardware::print() const
{
    std::cout << "screen: " << screen << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "mmWidth: " << mmWidth << std::endl;
    std::cout << "mmHeight: " << mmHeight << std::endl;
    std::cout << std::endl << std::endl;
}
