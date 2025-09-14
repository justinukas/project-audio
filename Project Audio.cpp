// Project Audio.cpp

#include "Project Audio.h"
#include "miniaudio.h"

volatile bool playing = false;
bool deviceOn = false;
float factor = 0.5f; // volume setting

// this is confusing too. i think its for decoding the audio
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL || !playing) {
        return;
    }

    ma_uint64 framesRead = 0;
    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    // Check if we've reached the end of the stream
    if (framesRead < frameCount) {
        playing = false;
        std::cout << "End of playback.\n";
    }

    // some voodoo shit for volume control from this github issue
    // https://github.com/mackron/miniaudio/issues/26#issuecomment-406415191
    for (size_t i = 0; i < framesRead; i++) {
        float* samples = (float*)pOutput;
        ma_uint32 channelCount = pDecoder->outputChannels;
        for (size_t c = 0; c < channelCount; c++) {
            samples[i * pDecoder->outputChannels + c] *= factor;
        }
    }
        
    (void)pInput; // unused
}

void cmnd_help() {
    std::cout
        << "Available commands:\n"
        << "help\n"
        << "load <file_directory>  -loads the audio file (only .wav, .mp3, and .flac are supported)\n"
        << "                       (HINT: you can drag and drop your file into the CLI!\n"
        << "play                   -start or resume playback\n"
        << "stop                   -stop playback\n"
        << "pause                  -pause playback\n"
        << "seek                   -seek to a specific timestamp. Syntax: mm:ss\n"
        << "exit                   -exit program\n";
}

void cmnd_load(std::string &strAudioPath, ma_result &result, ma_decoder &decoder, ma_device_config &deviceConfig, ma_device &device) {
    if (strAudioPath.empty()) {
        std::cout << "No file path provided\n";
        return;
    }

    // removes the quotation marks from the ends of the input string if any are present
    strAudioPath.erase(remove(strAudioPath.begin(), strAudioPath.end(), '\"'), strAudioPath.end());
    const char* audioPath = strAudioPath.c_str();

    if (playing) {
        playing = false;
        if (deviceOn) ma_device_stop(&device);
    }

    result = ma_decoder_init_file(audioPath, NULL, &decoder);
    if (result != MA_SUCCESS) {
        std::cout << "Could not load file: " << audioPath << "\n";
        return;
    }

    // device configuration
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;

    std::cout << "File " << audioPath << " loaded successfuly!\n";
}

void cmnd_play(ma_result &result, ma_decoder &decoder, ma_device_config &deviceConfig, ma_device &device) {
    if (playing) return;

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
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device\n";
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    deviceOn = true;
    playing = true;
    std::cout << "Playing... \n";
}

void cmnd_stoppause(ma_decoder &decoder, ma_device &device, std::string cmd) {
    if (playing == false) {
        std::cout << "Nothing is playing\n";
        return;
    }

    playing = false;
    if (deviceOn) ma_device_stop(&device);

    if (cmd == "stop") {
        ma_decoder_seek_to_pcm_frame(&decoder, 0);
        std::cout << "Stopped and rewound\n";
    }
    else if (cmd == "pause") {
        std::cout << "Paused\n";
    }
}

// this some chatgpt voodoo
bool validSeekSyntax(std::string &s) {
    std::regex pattern("^\\d{2}:\\d{2}$");
    return std::regex_match(s, pattern);
}

void cmnd_seek(ma_decoder& decoder, ma_device& device, std::string strLength) {
    if (!validSeekSyntax(strLength)) {
        std::cout << "Invalid syntax\n";
        return;
    }

    if (!playing) {
        std::cout << "Nothing is currently playing\n";
        return;
    }

    std::string strLengthSec, strLengthMin;
    int lengthSec, lengthMin, length;

    std::stringstream ss(strLength);
    std::getline(ss, strLengthMin, ':');
    std::getline(ss, strLengthSec);

    lengthMin = stoi(strLengthMin);
    lengthSec = stoi(strLengthSec);
    length = lengthMin * 60 + lengthSec;

    ma_uint64 seekFrame = static_cast<ma_uint64>(length) * decoder.outputSampleRate;;
    
    ma_device_stop(&device);
    ma_decoder_seek_to_pcm_frame(&decoder, seekFrame);
    ma_device_start(&device);
    // This is one way of doing it. The reason for the glitch is that decoders are not thread safe. 
    // So you'll end up in a situation where the device is reading from the decoder in it's data callback which is running on its own thread, 
    // and then in another thread you are seeking.
    // Another way you could do it is to do your reading and seeking all inside the same thread, i.e.the device's data callback. 
    // Alternatively you could use a mutex.
    // ^ from https://www.reddit.com/r/miniaudio/comments/1i253qu/how_can_i_seeking_a_single_audio_file_intilized/
}

bool containsLetters(std::string s) {
    bool contains = false;
    for (int i = 0; i < s.length(); i++) {
        if (isdigit(s[i]) == false) {
            contains = true;
            break;
        }
    }
    return contains;
}

void cmnd_volume(ma_decoder& decoder, ma_device& device, std::string strVolume) {
    float volume;

    if (containsLetters(strVolume) || strVolume == "") {
        std::cout << "Please input a number from 0-100\n";
        return;
    }

    volume = stof(strVolume);

    if (volume > 100 || volume < 0) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }

    factor = volume/100.0f;
}

int main() {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    std::string userInput, cmd, parameter;
    bool exitLoop = false;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    // while loop for user input
    while (exitLoop == false)
    {
        std::getline(std::cin, userInput);
        std::stringstream ss(userInput);
        std::getline(ss, cmd, ' ');

        if (cmd == "help") {
            cmnd_help();
        }
        else if (cmd == "load") {
            std::getline(ss, parameter);
            cmnd_load(parameter, result, decoder, deviceConfig, device);
        }
        else if (cmd == "play") {
            cmnd_play(result, decoder, deviceConfig, device);
        }
        else if (cmd == "stop" || cmd == "pause") {
            cmnd_stoppause(decoder, device, cmd);
        } 
        else if (cmd == "seek") {
            std::getline(ss, parameter);
            cmnd_seek(decoder, device, parameter);
        }
        else if (cmd == "volume") {
            std::getline(ss, parameter);
            cmnd_volume(decoder, device, parameter);
        }
        else if (cmd == "exit") {
            exitLoop = true;
        }

        // debug commands
        else if (cmd == "isplaying") {
            std::cout << std::boolalpha << playing << "\n";
        } 
        else {
            std::cout << "Unknown command. Type 'help' for available commands";
        }
    }
    std::cout << "Exiting... ";

    return 0;
}
