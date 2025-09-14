// utilities.cpp
#include "include/utilities.h"

bool playing = false;
float factor = 0.5f; // volume setting
std::mutex audioMutex;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder* pDecoder = static_cast<ma_decoder*>(pDevice->pUserData);
    if (pDecoder == NULL || !playing) return;

    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    if (framesRead < frameCount) {
        playing = false;
        std::cout << "End of playback\n";
    }

    // some voodoo shit for volume control from this github issue
    // https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
    float* samples = static_cast<float*>(pOutput);
    ma_uint32 channelCount = pDecoder->outputChannels;
    ma_uint64 totalSamples = framesRead * channelCount;
    for (size_t i = 0; i < totalSamples; i++) {
        samples[i] *= factor;
    }

    (void)pInput; // unused
}

void configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig) {

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;
}

void deviceCleanup(ma_decoder& decoder, ma_device& device) {
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
}

void seekToFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
}

bool validateVolumeInput(std::string& input, float& outVolume) {
    try {
        int vol = std::stoi(input);

        if (vol < 0 || vol > 100) return false;
        outVolume = vol / 100.0f; // convert % to decimal
        return true;
    }
    catch (...) {
        return false;
    }
}