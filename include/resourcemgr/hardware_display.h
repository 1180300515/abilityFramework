#ifndef _HARDWARE_DISPLAY_H
#define _HARDWARE_DISPLAY_H

#include "json/json.h"

class DisplayHardware
{
public:
    int screen;
    int x;
    int y;
    int width;
    int height;
    int mmWidth;
    int mmHeight;

    Json::Value toJson() const;
    void fromJson(const Json::Value &root);
    std::string ToKeyValue(std::string hostname) const;
    void print();
};

#endif // _HARDWARE_DISPLAY_H