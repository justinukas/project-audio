// commands.cpp
#include "include/commands.h"

extern bool playing;
extern float factor;

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

void cmnd_load(std::string& strAudioPath, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device) {
    if (playing) {
        playing = false;
        deviceCleanup(decoder, device);
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
        deviceCleanup(decoder, device);
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

void cmnd_volume(ma_decoder& decoder, ma_device& device, std::string strVolume) {
    float newVolume;
    if (!validateVolumeInput(strVolume, newVolume)) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }
    factor = newVolume;
}