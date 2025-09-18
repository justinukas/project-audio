// utilities.h
#pragma once

#include "miniaudio.h"
#include <string>
#include <iostream>
#include <mutex>

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
void configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig);
void deviceCleanup(ma_decoder& decoder, ma_device& device);
void seekToFrame(ma_decoder& decoder, ma_uint64 frame);
bool validateVolumeInput(std::string& input, float& outVolume);
void getTime(ma_decoder& decoder, int& seconds, int& minutes, std::string timeType);