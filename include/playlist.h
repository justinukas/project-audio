#pragma once

#include "miniaudio.h"

#include <string>
#include <map>

void static makePlaylistFile(std::string directory);
void static playPlaylist(std::string directory, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
std::map<int, std::string> readPlaylist(std::string directory);
void cmnd_playlist(std::string parameter, std::string& directory, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
void cmnd_skip(ma_decoder& decoder);
