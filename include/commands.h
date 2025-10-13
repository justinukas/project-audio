// commands.h
#pragma once

//#include "../miniaudio.c"
#include "miniaudio.h"
#include <filesystem>
#include "utilities.h"
#include <string>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

void cmnd_help();
void cmnd_load(std::string& strAudioPath, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device, bool& decoderInitialized);
void cmnd_play(ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device, bool decoderInitialized);
void cmnd_stoppause(ma_decoder& decoder, ma_device& device, std::string cmd);
void cmnd_seek(ma_decoder& decoder, ma_device& device, std::string strLength);
void cmnd_volume(ma_decoder& decoder, ma_device& device, std::string strVolume);
void cmnd_elapsedTime(ma_decoder& decoder);
void cmnd_readDirectory(std::string& directory);