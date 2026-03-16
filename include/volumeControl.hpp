#pragma once

#include "globalVars.hpp"
#include <iostream>

class VolumeControl {
private:
    bool validateVolumeInput(std::string inputVolume, float& outVolume) {
    try {
        int vol = std::stoi(inputVolume);
        std::cout << vol << std::endl;

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
public:
    void setVolume(std::string inputVolume) {
        float newVolume;
        if (!validateVolumeInput(inputVolume, newVolume)) {
            std::cout << "Please input a number from 0-100\n";
            return;
        }
        volumeMultiplier = newVolume;
    }
};
