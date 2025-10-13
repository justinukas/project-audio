// utilities.cpp
#include "include/utilities.h"

bool isPlaying = false;
float volumeMultiplier = 0.5f; // volume setting
std::mutex audioMutex;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder* pDecoder = static_cast<ma_decoder*>(pDevice->pUserData);
    if (pDecoder == NULL || !isPlaying) return;

    ma_uint64 audioFramesRead = 0;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &audioFramesRead);

    if (audioFramesRead < frameCount) {
        isPlaying = false;
        std::cout << "End of playback\n";
    }

    // some voodoo shit for volume control from this github issue
    // https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
    float* samples = static_cast<float*>(pOutput);
    ma_uint32 channelCount = pDecoder->outputChannels;
    ma_uint64 totalSamples = audioFramesRead * channelCount;
    for (size_t i = 0; i < totalSamples; i+=2) {
        samples[i] *= volumeMultiplier;
    }
}

void configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig) {

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;
}

void deviceCleanup(ma_decoder& decoder, ma_device& device, bool decoderInitialized) {
    if (ma_device_is_started(&device)) {
        ma_device_uninit(&device);
    }
    if (decoderInitialized) {
        ma_decoder_uninit(&decoder);
    }
}

void seekToFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
}

bool validateVolumeInput(std::string& inputVolume, float& outVolume) {
    try {
        int vol = std::stoi(inputVolume);

        if (vol < 0 || vol > 100) {
            return false;
        }
        outVolume = vol / 100.0f; // convert % to decimal
        return true;
    }
    catch (...) {
        return false;
    }
}

void getTime(ma_decoder& decoder, int& seconds, int& minutes, std::string type) {
    std::lock_guard<std::mutex> lock(audioMutex);

    ma_uint64 frames = 0;
    if (type == "elapsed") {
        ma_decoder_get_cursor_in_pcm_frames(&decoder, &frames);
    }
    else if (type == "length") {
        ma_decoder_get_length_in_pcm_frames(&decoder, &frames);
    }
    
    seconds = static_cast<int>(frames) / decoder.outputSampleRate;
    minutes = seconds / 60;
    seconds -= minutes * 60;
}