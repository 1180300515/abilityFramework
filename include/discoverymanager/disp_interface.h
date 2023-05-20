#ifndef _DISP_INTERFACE_H
#define _DISP_INTERFACE_H

#include <iostream>
#include <vector>
#include <json/json.h>

class DisplayInfo
{
public:
    int screen;
    int x;
    int y;
    int width;
    int height;
    int mmWidth;
    int mmHeight;

    Json::Value toJson() const {
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

    static DisplayInfo fromJson(const Json::Value& root) {
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
};


std::vector<DisplayInfo> getDisplayInfo();

#endif // _DISP_INTERFACE_H