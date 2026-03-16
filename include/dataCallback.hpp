#pragma once

#include "miniaudio.h"

void checkEndOfPlayback(ma_uint64 framesRead, ma_uint64 frameCount);
void setVolume(ma_uint64 frameCount, void* pOutput, ma_decoder* pDecoder);
void dataCallback(ma_device* pDevice, void* pFramesOut, const void* pInput, ma_uint32 frameCount);
