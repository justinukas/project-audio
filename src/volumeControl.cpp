#include "../include/volumeControl.h"

#include <iostream>

float volumeMultiplier = 0.5;

bool validateVolumeInput(std::string inputVolume, float& outVolume) {
    try {
        int vol = std::stoi(inputVolume);

        if (vol < 0 || vol > 100) {
            return false;
        }
        outVolume = vol / 100.0f; // convert % to decimal
        return true;
    }
    catch (...) {
        return false;
    }
}

void cmnd_volume(std::string inputVolume) {
    float newVolume;
    if (!validateVolumeInput(inputVolume, newVolume)) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }
    volumeMultiplier = newVolume;
}
