#ifndef _CAM_INTERFACE_H_
#define _CAM_INTERFACE_H_

#include <iostream>
#include <glob.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <linux/videodev2.h>

// 定义一个摄像头设备的类
class CameraHardware
{
public:
    std::string device_path;
    std::string driver;
    std::string card;
    std::string bus_info;
    std::vector<std::string> formats;
    //...更多属性和方法...

    CameraHardware &operator=(const CameraHardware &other)
    {
        if (this != &other)
        {
            device_path = other.device_path;
            driver = other.driver;
            card = other.card;
            bus_info = other.bus_info;
            formats = other.formats;
            //...复制其它成员...
        }
        return *this;
    }
};

std::vector<CameraHardware> getCameraInfo();

#endif // _CAM_INTERFACE_H_