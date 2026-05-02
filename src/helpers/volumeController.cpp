#pragma once

#include "../../include/outputProcessor.hpp"
#include "../../include/helpers/volumeController.hpp"
#include "../../include/audioMaster.hpp"

bool VolumeController::validateVolumeInput(std::string inputVolume, float& outVolume) {
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
void VolumeController::setVolume(std::string inputVolume, SharedAudioState& sharedState) {
    float newVolume;
    if (!validateVolumeInput(inputVolume, newVolume)) {
        msg("Please input a number from 0-100");
        return;
    }
    sharedState.volumeMultiplier.store(newVolume);
}
double VolumeController::currentVolume(SharedAudioState& sharedState) { return sharedState.volumeMultiplier.load(); }
