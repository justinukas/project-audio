/*#pragma once

#include "outputProcessor.hpp"
#include "audioMaster.hpp"

class VolumeController {
private:
    SharedAudioState* audioState;

    bool validateVolumeInput(std::string inputVolume, float& outVolume) {
    try {
        int vol = std::stoi(inputVolume);
        msg(inputVolume);

        if (vol < 0 || vol > 100) {
            return false;
        }
        outVolume = vol / 100.0; // convert % to decimal
        return true;
    }
    catch (...) {
        return false;
    }
}
public:
    VolumeController(SharedAudioState* s) : audioState(s){}

    void setVolume(std::string inputVolume) {
        float newVolume;
        if (!validateVolumeInput(inputVolume, newVolume)) {
            msg("Please input a number from 0-100");
            return;
        }
        audioState->volumeMultiplier = newVolume;
    }
    double currentVolume() { return audioState->volumeMultiplier.load(); }
};
*/