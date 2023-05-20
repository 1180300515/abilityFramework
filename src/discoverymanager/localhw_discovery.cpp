#include "discoverymanager/localhw_interface.h"
#include "discoverymanager/audio_interface.h"
#include "discoverymanager/disp_interface.h"



DeviceProfile getLocalHWInfo(){
    deviceProfile.cameraDevices = getCameraInfo();
    deviceProfile.displayDevices = getDisplayInfo();
    auto audioDevices = getAudioInfo();
    deviceProfile.micDevices = audioDevices.first;
    deviceProfile.speakerDevices = audioDevices.second;
    return deviceProfile;
}

void LocalhwPrint(){
    auto a =  getLocalHWInfo();
    for (const auto &device : a.micDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }

    for (const auto &device : a.speakerDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }

    for (const auto &device : a.cameraDevices)
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
    

    for (const auto &device : a.displayDevices)
    {
        std::cout << "  " << device.screen << std::endl;
        std::cout << "  " << device.x << std::endl;
        std::cout << "  " << device.y << std::endl;
        std::cout << "  " << device.width << std::endl;
        std::cout << "  " << device.height << std::endl;
        std::cout << "  " << device.mmWidth << std::endl;
        std::cout << "  " << device.mmHeight << std::endl;
    }
}
