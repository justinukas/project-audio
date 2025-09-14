// Project Audio.cpp

#include "miniaudio.h"
#include "Project Audio.h"

std::mutex audioMutex;
bool playing = false;
float factor = 0.5f; // volume setting

// this is confusing too. i think its for decoding the audio
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
        << "volume                 -set volume from 0-100\n"
        << "exit                   -exit program\n"
        << "\nDEBUG COMMANDS:\n"
        << "isplaying              -check if audio is playing\n";
}

void configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig) {
    
    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate = decoder.outputSampleRate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &decoder;
}

void cleanup(ma_decoder& decoder, ma_device& device) {
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);
}

void cmnd_load(std::string& strAudioPath, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    if (playing) {
        playing = false;
        cleanup(decoder, device);
    }

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

void seekToFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
}


void cmnd_play(ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    // seek to beginning
    if (playing) {
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
    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start playback device\n";
        cleanup(decoder, device);
        return;
    }

    playing = true;
    std::cout << "Playing... \n";
}

void cmnd_stoppause(ma_decoder& decoder, ma_device& device, std::string cmd) {
    if (playing) {
        ma_device_stop(&device);
        playing = false;
    }

    if (cmd == "stop") {
        seekToFrame(decoder, 0);
        std::cout << "Stopped and rewound\n";
    }
    else if (cmd == "pause") {
        std::cout << "Paused\n";
    }
}

void cmnd_seek(ma_decoder& decoder, ma_device& device, std::string strLength) {
    if (!playing) {
        std::cout << "Nothing is currently playing\n";
        return;
    }
    // check for syntax 'xx:xx'
    if (!std::regex_match(strLength, (std::regex)("^\\d{2}:\\d{2}$"))) {
        std::cout << "Invalid syntax\n";
        return;
    }

    int seconds, minutes, length;
    char colon;

    std::istringstream ss(strLength);
    ss >> minutes >> colon >> seconds;

    length = minutes * 60 + seconds;

    ma_uint64 seekFrame = static_cast<ma_uint64>(length) * decoder.outputSampleRate;;
    
    seekToFrame(decoder, seekFrame);
    // The decoders are not thread safe, so if you read while there is an active data callback which is running on its own thread
    // the audio bugs out because it moves threads.
    // Another way to implement this is to read and seek all inside the same thread, i.e.the data callback. 
    // Alternatively I can use a mutex (?).
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

bool validateVolumeInput(std::string& input, float& outVolume) {
    try {
        float vol = std::stof(input);

        if (vol < 0.0f || vol > 100.0f) return false;
        outVolume = vol / 100.0f; // convert % to decimal
        return true;
    }
    catch (...) {
        return false;
    }
}

void cmnd_volume(ma_decoder& decoder, ma_device& device, std::string strVolume) {
    float newVolume;
    if (!validateVolumeInput(strVolume, newVolume)) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }
    factor = newVolume;
}

int main() {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    std::string userInput, cmd, parameter;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    // while loop for user input
    while (true)
    {
        std::getline(std::cin, userInput);
        std::stringstream ss(userInput);
        std::getline(ss, cmd, ' ');
        std::getline(ss, parameter);

        if (cmd == "help")cmnd_help();
        else if (cmd == "load") cmnd_load(parameter, result, decoder, deviceConfig, device);
        else if (cmd == "play") cmnd_play(result, decoder, deviceConfig, device);
        else if (cmd == "stop" || cmd == "pause") cmnd_stoppause(decoder, device, cmd);
        else if (cmd == "seek") cmnd_seek(decoder, device, parameter);
        else if (cmd == "volume") cmnd_volume(decoder, device, parameter);
        else if (cmd == "exit") break;
        else if (cmd == "isplaying") std::cout << std::boolalpha << playing << "\n";
        else std::cout << "Unknown command. Type 'help' for available commands\n";
    }
    std::cout << "Exiting... ";

    if (playing) {
        ma_device_stop(&device);
        playing = false;
    }
    cleanup(decoder, device);

    return 0;
}
