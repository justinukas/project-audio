#pragma once

#include <string>

extern float volumeMultiplier;

bool validateVolumeInput(std::string inputVolume, float& outVolume);
void cmnd_volume(std::string inputVolume);
