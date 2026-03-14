#pragma once

#include "miniaudio.h"

#include <string>

void seekFrame(ma_decoder& decoder, ma_uint64 frame);
ma_uint64 frameToSeek(std::string strLength, ma_uint32 outputSampleRate);
void cmnd_seek(std::string strLength, ma_decoder& decoder, ma_device& device);
