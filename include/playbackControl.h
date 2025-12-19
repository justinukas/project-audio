#pragma once

#include "miniaudio.h"
#include <string>

extern bool paused;

void cleanup(ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized);
void cmnd_load(std::string& rawUserInput, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
void static configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig);
void static initializeDevice(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig);
void cmnd_play(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
void cmnd_stopPause(std::string input, ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized);
