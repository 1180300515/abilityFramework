#include <pulse/pulseaudio.h>
#include <iostream>
#include <string>
#include <vector>

class AudioDevice {
public:
    std::string name;
    std::string description;
    uint32_t index;
    uint32_t channels;

    AudioDevice(std::string name, std::string description, uint32_t index, uint32_t channels) :
        name(name), description(description), index(index), channels(channels) {}

    void print() {
        std::cout << "name: " << name << ", description: " << description << ", index: " << index << ", channels: " << channels << std::endl;
    }
};

class AudioQueue {
public:
    std::vector<AudioDevice> SourceDevice;
    std::vector<AudioDevice> SinkDevice;

    void print() {
        std::cout << "录音设备列表：" << std::endl;
        for (auto& device : SourceDevice) {
            device.print();
        }
        std::cout << "播放设备列表：" << std::endl;
        for (auto& device : SinkDevice) {
            device.print();
        }
    }
};

AudioQueue AuQueue;

void pa_get_device_list() {
    int error;
    pa_mainloop* mainloop = pa_mainloop_new();
    pa_mainloop_api* mainloop_api = pa_mainloop_get_api(mainloop);
    pa_context* context = pa_context_new(mainloop_api, "PulseAudio Test");

    pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL);
    pa_mainloop_run(mainloop, &error);

    pa_operation* op;
    op = pa_context_get_sink_info_list(context, [](pa_context* context, const pa_sink_info* info, int eol, void* userdata) {
        if (eol == 0) {
            AudioDevice new_device(info->name, info->description, info->index, info->sample_spec.channels);
            AuQueue.SinkDevice.push_back(new_device);
        }
    }, NULL);
    pa_operation_unref(op);

    op = pa_context_get_source_info_list(context, [](pa_context* context, const pa_source_info* info, int eol, void* userdata) {
        if (eol == 0) {
            AudioDevice new_device(info->name, info->description, info->index, info->sample_spec.channels);
            AuQueue.SourceDevice.push_back(new_device);
        }
    }, NULL);
    pa_operation_unref(op);

    pa_context_disconnect(context);
    pa_context_unref(context);
    pa_mainloop_free(mainloop);
}

int main() {
    pa_get_device_list();
    AuQueue.print();
    return 0;
}
