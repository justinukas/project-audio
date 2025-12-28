#include "../include/dataCallback.h"
#include "../include/volumeControl.h"
#include "../include/playlist.h"

#include <iostream>

std::mutex audioMutex;  
bool soundIsPlaying = false;

void static checkEndOfPlayback(ma_uint64 framesRead, ma_uint64 frameCount) {
	if (framesRead < frameCount) {
		soundIsPlaying = false;

		playNextSound.notify_one(); // chatgpt voodoo

		std::cout << "Playback finished!\n";
	}
}

// some voodoo for volume control from this github issue
// https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
void static setVolume(ma_uint64 frameCount, void* pOutput, ma_decoder* pDecoder) {
	float* samples = static_cast<float*>(pOutput);
	ma_uint32 channelCount = pDecoder->outputChannels;

	ma_uint64 totalSamples = frameCount * channelCount;
	for (size_t i = 0; i < totalSamples; i++) {
		samples[i] *= volumeMultiplier;
	}
	// i guess the size of a sample's element signifies the volume of it
}

void data_callback(ma_device* pDevice, void* pFramesOut, const void* pInput, ma_uint32 frameCount) {
	std::lock_guard<std::mutex> lock(audioMutex);

	ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
	if (pDecoder == NULL || soundIsPlaying == false) {
		return;
	}

	ma_uint64 framesRead = 0;
	ma_decoder_read_pcm_frames(pDecoder, pFramesOut, frameCount, &framesRead); // playback of audio

	checkEndOfPlayback(framesRead, frameCount);
	setVolume(framesRead, pFramesOut, pDecoder);
}
