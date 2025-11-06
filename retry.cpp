#include "include/main.h"

std::mutex audioMutex;          // use this when reading frames outside of data callback in any way (stuff like seeking)
bool playbackFinished = true;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    std::lock_guard<std::mutex> lock(audioMutex);

    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL || playbackFinished == true) {
        return;
    }

    ma_uint64 framesRead = 0;
    // playback of audio
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    if (framesRead < frameCount) {
        playbackFinished = true;
        std::cout << "Playback finished!\n";
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

void seekToFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
}

void cmnd_load(std::string& strAudioPath, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    //ma_decoder_uninit(&decoder);

    if (strAudioPath.empty()) {
        std::cout << "No file path provided\n";
        return;
    }

    // removes the quotation marks from the ends of the input string if any are present
    strAudioPath.erase(remove(strAudioPath.begin(), strAudioPath.end(), '\"'), strAudioPath.end());
    const char* audioPath = strAudioPath.c_str();

    result = ma_decoder_init_file(audioPath, NULL, &decoder);

    if (result != MA_SUCCESS) {
        std::cout << "Could not load file: " << audioPath << "\n";
        return;
    }

    configureDevice(decoder, deviceConfig);

    std::cout << "File " << audioPath << " loaded successfuly!\n";
}

void cmnd_play(ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    // start playback from start of file if sth is already playing
    if (!playbackFinished) {
        seekToFrame(decoder, 0);
        return;
    }

    if (result != MA_SUCCESS) {
        std::cout << "Please load a file using 'load <file_path>' first!\n";
        return;
    }

    // these ifs below are if statements and also function calls at the same time somehow
    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        std::cout << "Failed to open playback device\n";
        ma_decoder_uninit(&decoder);
        return;
    }

    playbackFinished = false;

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    std::cout << "Playing... \n";    
}

void cmnd_skip(ma_decoder& decoder) {
    if (playbackFinished) {
        return;
    }
    ma_uint64 totalFrames = 0;
    ma_decoder_get_length_in_pcm_frames(&decoder, &totalFrames);
    seekToFrame(decoder, totalFrames-1);
}

// cleanup devices after end of playback
void cleanup(ma_result& result, ma_decoder& decoder, ma_device& device) {
    while (true) {
        if (playbackFinished && ma_device_is_started(&device) /*&& !doLoop*/) {
            ma_device_uninit(&device);
            ma_decoder_uninit(&decoder);
            ma_device_stop(&device);
            result = MA_ERROR;
            std::cout << "Cleaned up\n";
        }
        /*else if (playbackFinished && doLoop) {
            ma_decoder_uninit(&decoder)
            result = MA_ERROR;
        }*/
    }
}

int main()
{
    ma_result result = MA_ERROR;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    std::thread (cleanup, std::ref(result), std::ref(decoder), std::ref(device)).detach();

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    // while loop for user input
    while (true)
    {
        std::string userInput, cmnd, parameter;

        std::getline(std::cin, userInput);
        std::stringstream ss(userInput);

        // split userinput into command and the paramater of said command
        std::getline(ss, cmnd, ' ');
        std::getline(ss, parameter);

        if (cmnd == "load") cmnd_load(parameter, result, decoder, deviceConfig, device);
        else if (cmnd == "play") {
            cmnd_play(result, decoder, deviceConfig, device);
            //std::thread(cmnd_play, std::ref(result), std::ref(decoder), std::ref(deviceConfig), std::ref(device)).detach();
        }
        else if (cmnd == "skip") cmnd_skip(decoder);
        else if (cmnd == "exit") break;

        else std::cout << "Unknown command. Type 'help' for available commands\n";
    }
    std::cout << "Exiting... ";

    if (ma_device_is_started(&device)) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
    }
    return 0;
}