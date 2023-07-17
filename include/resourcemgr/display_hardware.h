#ifndef DISPLAY_HARDWARE_H
#define DISPLAY_HARDWARE_H

#include "json/json.h"

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

    Json::Value toJson() const;
    static DisplayInfo fromJson(const Json::Value &root);
    void print();
};

#endif // DISPLAY_HARDWARE_H