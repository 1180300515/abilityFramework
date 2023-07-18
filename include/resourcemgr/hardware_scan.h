#ifndef HARDWARE_SCAN_H
#define HARDWARE_SCAN_H

#include <pulse/pulseaudio.h>
#include <functional>

#include "device_profile.h"

class HardwareScan
{
public:
    /**
     * @brief init the hardwarescan
     * @param callback add device callback function
     */
    void Init(std::function<bool(std::string, bool)> callback, std::string hostname);
    /**
     * @brief base the hardware info to generate CR
     */
    void AutoGenerateCR();
    /**
     * @brief get the local hardware device json string
     * @param format format string or not
     * @return the json string
     */
    std::string GetHardwareDeviceInfo(bool format);

private:
    /**
     * @brief scan the local hardware
     */
    void localHardwareScan();
    void insertCameraInfo(Json::Value &jnode);
    void insertMicInfo(Json::Value &jnode);
    void insertLoudspeakerInfo(Json::Value &jnode);
    /**
     * @brief get the camera hardware info
     */
    void getCameraInfo();
    /**
     * @brief get the display hardware info
     */
    void getDisplayInfo();
    /**
     * @brief get the audio (mic and speaker) hardware info
     */
    void getAudioInfo();

    // callback function used for hardware scan
    static void context_state_callback(pa_context *c, void *userdata);
    static void source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata);
    static void sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata);
    // store the callback data
    static std::vector<AudioDevice> loudspeakerdevices;
    static std::vector<AudioDevice> microphonedevices;

    // called when need to add device instance
    std::function<bool(std::string, bool)> add_device_callback;
    DeviceProfile profile;

    std::string hostname_;
};

#endif // HARDWARE_SCAN_H