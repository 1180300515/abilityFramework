#include "discoverymanager/audio_interface.h"

static void sink_info_callback(pa_context *c, const pa_sink_info *info, int eol, void *userdata)
{
    if (eol > 0)
        return;

    // std::cout << info->name << std::endl;
    // sinki = sinki + 1;
    // std::cout << "sink channels: " << info->sample_spec.channels << std::endl;
    if (info && !strstr(info->name, "monitor"))
    {
        outputDevices.emplace_back(info->name, info->description, pa_cvolume_avg(&(info->volume)), info->mute, info->sample_spec.rate, info->sample_spec.channels);
    }
}

static void source_info_callback(pa_context *c, const pa_source_info *info, int eol, void *userdata)
{
    if (eol > 0)
        return;

    // std::cout << info->name << std::endl;
    // sourcei = sourcei + 1;
    // std::cout << "source channels: " << info->sample_spec.channels << std::endl;
    if (info && !strstr(info->name, "monitor"))
    {
        inputDevices.emplace_back(info->name, info->description, pa_cvolume_avg(&(info->volume)), info->mute, info->sample_spec.rate, info->sample_spec.channels);
    }
}

static void context_state_callback(pa_context *c, void *userdata)
{
    if (pa_context_get_state(c) == PA_CONTEXT_READY)
    {
        pa_operation *o;
        std::cout << "ready" << std::endl;

        o = pa_context_get_sink_info_list(c, sink_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }

        std::cout << "ccc " << std::endl;

        o = pa_context_get_source_info_list(c, source_info_callback, nullptr);
        if (o)
        {
            pa_operation_unref(o);
        }
        // counter = counter + 1;
        // std::cout << "counter : " << counter << std::endl;
    }
    // std::cout << "status: " << pa_context_get_state(c) << std::endl;
    // #define PA_CONTEXT_UNCONNECTED PA_CONTEXT_UNCONNECTED    0
    // #define PA_CONTEXT_CONNECTING PA_CONTEXT_CONNECTING      1
    // #define PA_CONTEXT_AUTHORIZING PA_CONTEXT_AUTHORIZING    2
    // #define PA_CONTEXT_SETTING_NAME PA_CONTEXT_SETTING_NAME  3
    // #define PA_CONTEXT_READY PA_CONTEXT_READY                4
    // #define PA_CONTEXT_FAILED PA_CONTEXT_FAILED              5
    // #define PA_CONTEXT_TERMINATED PA_CONTEXT_TERMINATED      6
}

std::pair<std::vector<AudioDevice>, std::vector<AudioDevice>> getAudioInfo()
{
    pa_threaded_mainloop *mainloop = pa_threaded_mainloop_new();
    pa_mainloop_api *mainloop_api = pa_threaded_mainloop_get_api(mainloop);
    pa_context *context = pa_context_new(mainloop_api, "Device Details");

    pa_context_set_state_callback(context, context_state_callback, nullptr);
    std::cout << "connect " << std::endl;
    pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr);

    std::cout << "Before run" << std::endl;
    pa_threaded_mainloop_start(mainloop);
    // usleep(2000 * 1000);
    // std::cout << "Quit the loop." << std::endl;
    // pa_mainloop_quit(mainloop, 0);

    // pa_context_disconnect(context);
    // pa_context_unref(context);
    // pa_mainloop_free(mainloop);

    sleep(2);
    pa_threaded_mainloop_stop(mainloop);

    std::cout << "Output devices:" << std::endl;
    for (const auto &device : outputDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }

    std::cout << "Input devices:" << std::endl;
    for (const auto &device : inputDevices)
    {
        std::cout << "  " << device.description << std::endl;
        std::cout << "  " << device.name << std::endl;
        std::cout << "  " << device.volume << std::endl;
        std::cout << "  " << device.mute << std::endl;
        std::cout << "  " << device.sampleRate << std::endl;
        std::cout << "  " << device.channels << std::endl;
    }
    return std::make_pair(inputDevices, outputDevices);
}
