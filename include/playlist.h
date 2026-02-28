#pragma once

#include "miniaudio.h"


#include <string>
#include <map>
#include <filesystem>
#include <condition_variable>
#include <filesystem>

extern std::condition_variable playNextSound;

void static makePlaylistFile(std::filesystem::path path, std::filesystem::path fullPath);
void static playPlaylist(std::filesystem::path fullPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
std::map<int, std::string> static readPlaylist(std::filesystem::path fullPath);
void cmnd_playlist(std::string parameter, std::filesystem::path givenPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
void cmnd_skip(ma_decoder& decoder);
