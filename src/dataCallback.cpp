#include "../include/dataCallback.hpp"
#include "../include/outputProcessor.hpp"
#include "../include/audioMaster.hpp"

#include <mutex>

void DataCallback::checkEndOfPlayback(ma_uint64 framesRead, ma_uint64 frameCount, AudioMaster* master) {
    if (framesRead < frameCount) {
			
		master->statePointer()->soundIsPlaying.store(false);
		master->statePointer()->playbackFinished.store(true);

		msg("Playback finished!");
	}
}

// some voodoo for volume control from this github issue
// https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
void DataCallback::applyVolume(ma_uint64 frameCount, void* pFramesOut, ma_decoder* pDecoder, double volumeMultiplier) {
	float* samples = static_cast<float*>(pFramesOut);
	ma_uint32 channelCount = pDecoder->outputChannels;

	ma_uint64 totalSamples = frameCount * channelCount;
	for (size_t i = 0; i < totalSamples; i++) {
		samples[i] *= volumeMultiplier;
    }
	// I guess the size of a sample's element signifies the volume of it
}

void DataCallback::dataCallback(ma_device* pDevice, void* pFramesOut, const void* pInput, ma_uint32 frameCount) {
	//std::mutex audioMutex;
	//std::lock_guard<std::mutex> lock(audioMutex);

	AudioMaster* master = static_cast<AudioMaster*>(pDevice->pUserData); 
	ma_decoder* decoder = master->decoder.decoderPointer();
	if (decoder == NULL || master->statePointer()->soundIsPlaying.load() == false) {
		return;
	}

	ma_uint64 framesRead = 0; //master->decoder.readFrames(pFramesOut, frameCount, &framesRead);
	ma_decoder_read_pcm_frames(decoder, pFramesOut, frameCount, &framesRead);
	msg(std::to_string(frameCount));
	msg(std::to_string(framesRead));

	checkEndOfPlayback(framesRead, frameCount, master);
	applyVolume(frameCount, pFramesOut, decoder, master->volumeController.currentVolume(*master->statePointer()));
}
