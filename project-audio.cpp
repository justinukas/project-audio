#include "include/main.h"

std::mutex audioMutex;          // use this when reading frames outside of data callback in any way (stuff like seeking)
bool soundIsPlaying = false;
float volumeMultiplier = 0.5;

void checkEndOfPlayback(ma_uint64 framesRead, ma_uint32 frameCount) {
    //soundIsPlaying = framesRead < frameCount;   <-- can write it like this instead later if i want
    if (framesRead < frameCount) {
        soundIsPlaying = false;
        std::cout << "Playback finished!\n";
    }
}

// some voodoo for volume control from this github issue
// https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
void controlVolume(ma_uint64 framesRead, void* pOutput, ma_decoder* pDecoder) {
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
    controlVolume(framesRead, pOutput, pDecoder);
}

void cmnd_load(std::string& rawUserInput, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
    if (ma_device_is_started(&device)) {
        ma_device_uninit(&device);
        soundIsPlaying = false;
    }

    if (rawUserInput.empty()) {
        std::cout << "No file path provided\n";
        return;
    }

    rawUserInput.erase(remove(rawUserInput.begin(), rawUserInput.end(), '\"'), rawUserInput.end());
    const char* audioPath = rawUserInput.c_str();

    decoderInitialized = ma_decoder_init_file(audioPath, NULL, &decoder);

    if (decoderInitialized != MA_SUCCESS) {
        std::cout << "Could not load file: \"" << audioPath << "\"\n";
        return;
    }

    std::cout << "File " << audioPath << " loaded successfuly!\n";
}

void seekFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
}

void configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig) {
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;
}

void initializeDevice(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig) {
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "Failed to open playback device\n";
        ma_decoder_uninit(&decoder);
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }
}

void cmnd_play(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
    // start from the beginning if something is already playing
    if (soundIsPlaying) {
        const ma_uint64 beginningFrame = 0;
        seekFrame(decoder, beginningFrame);
        return;
    }

    if (decoderInitialized != MA_SUCCESS) {
        std::cout << "Please load a file using 'load <file_path>' first!\n";
        return;
    }

    configureDevice(decoder, deviceConfig);
    initializeDevice(decoder, device, deviceConfig);

    soundIsPlaying = true;

    std::cout << "Playing..." + '\n';
}

bool validateVolumeInput(std::string inputVolume, float& outVolume) {
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

void cmnd_volume(std::string inputVolume) {
    float newVolume;
    if (!validateVolumeInput(inputVolume, newVolume)) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }
    volumeMultiplier = newVolume;
}

ma_uint64 frameToSeek(std::string strLength, ma_uint32 outputSampleRate) {
    int seconds, minutes, totalLengthSeconds;
    char colon;
    ma_uint64 frame;

    std::istringstream ss(strLength);
    ss >> minutes >> colon >> seconds;
    
    totalLengthSeconds = minutes * 60 + seconds;

    frame = static_cast<ma_uint64>(totalLengthSeconds) * outputSampleRate;
    return frame;
}

void cmnd_seek(std::string strLength, ma_decoder& decoder, ma_device& device) {
    if (!soundIsPlaying) {
        std::cout << "Nothing is currently playing\n";
        return;
    }

    // check for syntax 'mm:ss'
    if (!std::regex_match(strLength, (std::regex)("^\\d{2}:\\d{2}$"))) {
        std::cout << "Invalid syntax\n";
        return;
    }

    seekFrame(decoder, frameToSeek(strLength, decoder.outputSampleRate));
}

struct Command {
    std::string name;
    std::string parameter;
};

Command getInput() {
    std::string userInput, cmnd, parameter;

    std::getline(std::cin, userInput);
    std::istringstream iss(userInput);

    std::getline(iss, cmnd, ' ');
    std::getline(iss, parameter);

    return {cmnd, parameter};
}

void handleCommands(Command cmnd, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
    if (cmnd.name == "load") cmnd_load(cmnd.parameter, decoder, device, deviceConfig, decoderInitialized);
    else if (cmnd.name == "play") cmnd_play(decoder, device, deviceConfig, decoderInitialized);
    else if (cmnd.name == "seek") cmnd_seek(cmnd.parameter, decoder, device);
    else if (cmnd.name == "volume") cmnd_volume(cmnd.parameter);

    else std::cout << "Unknown command. Type 'help' for available commands\n";
}

void processUserInput(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
    while (true)
    {
        Command cmnd = getInput();
        if (cmnd.name == "exit") break;
        handleCommands(cmnd, decoder, device, deviceConfig, decoderInitialized);
    }
}

// cleanup devices after end of playback
void cleanup(ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized) {
    while (true) {
        const auto pauseInterval = std::chrono::milliseconds(1);
        std::this_thread::sleep_for(pauseInterval);

        // clean up devices if end of playback and the device is started
        if (!soundIsPlaying && ma_device_is_started(&device)) {
            ma_device_stop(&device);
            ma_device_uninit(&device);
            ma_decoder_uninit(&decoder);
            decoderInitialized = MA_ERROR;
            std::cout << "Cleaned up\n";
        }
    }
}

int main() {
    ma_decoder decoder;
    ma_device device;
    ma_device_config deviceConfig;
    ma_result decoderInitialized = MA_ERROR;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    
    std::thread(cleanup, std::ref(device), std::ref(decoder), std::ref(decoderInitialized)).detach();
    processUserInput(decoder, device, deviceConfig, decoderInitialized);
    
    std::cout << "Exiting... ";
}
