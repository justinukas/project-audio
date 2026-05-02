#pragma once

#include "../outputProcessor.hpp"

class SharedAudioState;

class VolumeController {
private:
    bool validateVolumeInput(std::string inputVolume, float& outVolume);
public:
    void setVolume(std::string inputVolume, SharedAudioState& sharedState);
    double currentVolume(SharedAudioState& sharedState);
};
