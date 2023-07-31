#ifndef _HARDWARE_SCAN_H
#define _HARDWARE_SCAN_H

#include <pulse/pulseaudio.h>
#include <functional>
#include <thread>
#include <mutex>
#include <optional>

#include "hardware_device_profile.h"

class ResourceManager;
class HardwareScan
{
public:
    /**
     * @brief init the hardwarescan
     * @param callback add device callback function
     */
    void Init(std::shared_ptr<ResourceManager> manager_, std::string hostname);
    /**
     * @brief start the periodic hardware scan thread
     */
    void Run();
    void Wait();
    /**
     * @brief get the local hardware device json string
     * @param format format string or not
     * @return the json string
     */
    std::string GetHardwareDeviceInfo(bool format);

    std::optional<CameraHardware> GetCameraHardware(const std::string &id, const std::string &key);

    std::optional<AudioHardware> GetMicHardware(const std::string &id, const std::string &key);

    std::optional<AudioHardware> GetSpeakerHardware(const std::string &id, const std::string &key);

    bool isAutogenInstance(const std::string &key);

    std::string GetMatchedKey(const std::string &id);

    void SetMap(const std::string &id, const std::string &key);

    void DeleteMap(const std::string &key);

private:
    /**
     * @brief scan the local hardware
     */
    void localHardwareScan();
    /**
     * @brief get the camera hardware info
     */
    void getCameraInfo();
    /**
     * @brief get the display hardware info
     */
    void getDisplayHardware();
    /**
     * @brief get the audio (mic and speaker) hardware info
     */
    void getAudioInfo();

    /**
     * @brief periodic hardware scan
     */
    void periodicHardwareScanThread();

    /**
     * @brief print the hardware and instance map
     */
    void printMap();
    /**
     * @brief delete the noexist hadware
     */
    void compareMap();

    /**
     * @brief generate a autogenCR serial number(only called by AutoGenerateCR)
     * @param type the instance type
     * @return number
     */
    int gnerateSerialNumber(const std::string &type);

    /**
     * @brief find the min positive not in the vector
     * @param nums 
     * @return 
     */
    int firstMissingPositive(std::vector<int> &nums);

    /**
     * @brief base the device profile,generate the key-value
     * @param camera
     * @param mic
     * @param speaker
     */
    void generateIndentifyKeyValue(std::map<std::string, CameraHardware> &camera,
                                   std::map<std::string, AudioHardware> &mic,
                                   std::map<std::string, AudioHardware> &speaker);
    /**
     * @brief base the hardware info to generate CR
     */
    void AutoGenerateCR();

    // callback function used for hardware scan
    static void context_state_callback(pa_context *c, void *userdata);
    static void source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata);
    static void sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata);
    // store the callback data
    static std::vector<AudioHardware> loudspeakerdevices;
    static std::vector<AudioHardware> microphonedevices;

    // called resource manager when need to add device instance
    std::shared_ptr<ResourceManager> resource_manager;

    DeviceProfile profile;
    std::mutex camera_hardware_lock_;
    std::mutex audio_hardware_lock_;
    std::mutex display_hardware_lock_;

    std::string hostname_;

    std::thread periodic_scan_thread;

    std::map<std::string, std::string> hardware_instance_map; // record the hardware identifier -> the instance key
    std::mutex map_lock_;
};

#endif // _HARDWARE_SCAN_H