#ifndef _AUDIO_INTERFACE_H_
#define _AUDIO_INTERFACE_H_

#include <string>



class AudioDevice{
public:
    std::string name;
    std::string description;
    int index;
    int channels;
};

class AudioInterface{
public:
    std::vector<AudioDevice> AudioDevices;
    virtual void getAudioDevices() = 0;
};


#endif // _AUDIO_INTERFACE_H_