#pragma once

#include "miniaudio.h"

#include <mutex>

extern std::mutex audioMutex; // use this when reading frames outside of data callback in any way (like seeking)
extern bool soundIsPlaying; 

void static checkEndOfPlayback(ma_uint64 framesRead, ma_uint32 frameCount);
void static setVolume(ma_uint64 framesRead, void* pOutput, ma_decoder* pDecoder);
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
