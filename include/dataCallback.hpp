#pragma once

#include "../libraries/miniaudio.h"
//#include "audioMaster.hpp"



class DataCallback {
private:
	class AudioMaster{};
	static void checkEndOfPlayback(ma_uint64 framesRead, ma_uint64 frameCount, AudioMaster* master);

	static void applyVolume(ma_uint64 frameCount, void* pFramesOut, ma_decoder* pDecoder, double volumeMultiplier);
public:
	static void dataCallback(ma_device* pDevice, void* pFramesOut, const void* pInput, ma_uint32 frameCount);
};
