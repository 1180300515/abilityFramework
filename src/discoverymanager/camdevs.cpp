#include "discoverymanager/cam_interface.h"

std::vector<CameraDevice> getCameraInfo()
{
    glob_t glob_result;
    std::vector<CameraDevice> camera_devices;

    // 找到所有的V4L设备
    glob("/dev/video*", GLOB_TILDE, nullptr, &glob_result);
    for (unsigned int i = 0; i < glob_result.gl_pathc; ++i)
    {
        const char *path = glob_result.gl_pathv[i];
        int fd = open(path, O_RDONLY);

        if (fd == -1)
        {
            std::cerr << "Failed to open " << path << std::endl;
            continue;
        }

        // 获取设备的能力
        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
        {
            std::cerr << "Failed to get device capabilities for " << path << std::endl;
            close(fd);
            continue;
        }

        // 枚举设备支持的像素格式
        v4l2_fmtdesc fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        char fourcc[5] = {0};
        std::vector<std::string> formats;
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) == 0)
        {
            strncpy(fourcc, reinterpret_cast<const char *>(&fmt.pixelformat), 4);
            formats.push_back(fourcc);
            fmt.index++;
        }

        // 创建一个CameraDevice对象，并添加到列表中
        CameraDevice device;
        device.device_path = path;
        device.driver = reinterpret_cast<const char *>(cap.driver);
        device.card = reinterpret_cast<const char *>(cap.card);
        device.bus_info = reinterpret_cast<const char *>(cap.bus_info);
        device.formats = formats;
        // camera_devices.push_back(device);

        bool device_exists = false;
        for (auto &existing_device : camera_devices)
        {
            if (existing_device.bus_info == device.bus_info)
            {
                device_exists = true;
                if (existing_device.device_path > device.device_path)
                {
                    existing_device = device; // Note: this requires CameraDevice to be assignable
                }
                break;
            }
        }

        if (!device_exists)
        {
            camera_devices.push_back(device);
        }

        close(fd);
    }

    globfree(&glob_result);

    // 打印摄像头设备列表
    for (const auto &device : camera_devices)
    {
        std::cout << "Device path: " << device.device_path << std::endl
                  << "Driver: " << device.driver << std::endl
                  << "Card: " << device.card << std::endl
                  << "Bus info: " << device.bus_info << std::endl
                  << "Supported formats: ";
        for (const auto &format : device.formats)
        {
            std::cout << format << " ";
        }
        std::cout << std::endl
                  << std::endl;
    }

    return camera_devices;
}