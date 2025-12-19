#include "../include/dataCallback.h"
#include "../include/volumeControl.h"

#include <iostream>

std::mutex audioMutex;
std::condition_variable playbackFinished;   // chatgpt voodoo
bool soundIsPlaying = false;

void static checkEndOfPlayback(ma_uint64 framesRead, ma_uint32 frameCount) {
    if (framesRead < frameCount) {
        //std::lock_guard<std::mutex> lock(audioMutex);
        soundIsPlaying = false;

        playbackFinished.notify_one(); // chatgpt voodoo

        std::cout << "Playback finished!\n";
    }
}

// some voodoo for volume control from this github issue
// https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
void static setVolume(ma_uint64 framesRead, void* pOutput, ma_decoder* pDecoder) {
    float* samples = static_cast<float*>(pOutput);
    ma_uint32 channelCount = pDecoder->outputChannels;

    ma_uint64 totalSamples = framesRead * channelCount;
    for (size_t i = 0; i < totalSamples; i++) {
        samples[i] *= volumeMultiplier;
    }
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    std::lock_guard<std::mutex> lock(audioMutex);

    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL || soundIsPlaying == false) {
        return;
    }

    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead); // playback of audio

    checkEndOfPlayback(framesRead, frameCount);
    setVolume(framesRead, pOutput, pDecoder);
}
