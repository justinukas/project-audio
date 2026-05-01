#pragma once

#include "../libraries/miniaudio.h"
//#include "audioMaster.hpp"

class AudioMaster;

class DataCallback {
private:
	static void checkEndOfPlayback(ma_uint64 framesRead, ma_uint64 frameCount, AudioMaster* master);

	// some voodoo for volume control from this github issue
	// https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
	static void applyVolume(ma_uint64 frameCount, void* pFramesOut, ma_decoder* pDecoder, double volumeMultiplier);
public:
	static void dataCallback(ma_device* pDevice, void* pFramesOut, const void* pInput, ma_uint32 frameCount);
};
