#include "display_hardware.h"

#include <iostream>

Json::Value DisplayInfo::toJson() const
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

DisplayInfo DisplayInfo::fromJson(const Json::Value &root)
{
    DisplayInfo disp;
    disp.screen = root["screen"].asInt();
    disp.x = root["x"].asInt();
    disp.y = root["y"].asInt();
    disp.width = root["width"].asInt();
    disp.height = root["height"].asInt();
    disp.mmWidth = root["mmWidth"].asInt();
    disp.mmHeight = root["mmHeight"].asInt();
    return disp;
}

void DisplayInfo::print()
{
    std::cout << "screen: " << screen << std::endl;
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
    std::cout << "width: " << width << std::endl;
    std::cout << "height: " << height << std::endl;
    std::cout << "mmWidth: " << mmWidth << std::endl;
    std::cout << "mmHeight: " << mmHeight << std::endl;
}
