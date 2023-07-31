#include "hardware_display.h"

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

std::string DisplayHardware::ToKeyValue(std::string hostname) const
{
    std::stringstream ss;
    ss << "hostname:" << hostname << ",";
    ss << "screen:" << screen << ",";
    ss << "x:" << x << ",";
    ss << "y:" << y << ",";
    ss << "width:" << width << ",";
    ss << "height:" << height << ",";
    ss << "mmWidth:" << mmWidth << ",";
    ss << "mmHeight:" << mmHeight;
    return ss.str();
}

void DisplayHardware::print()
{
    std::cout << "screen: " << screen << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "mmWidth: " << mmWidth << std::endl;
    std::cout << "mmHeight: " << mmHeight << std::endl;
}
