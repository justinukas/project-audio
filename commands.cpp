// commands.cpp
#include "include/commands.h"

extern bool isPlaying;
extern float volumeMultiplier;
extern std::mutex audioMutex;

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
        << "time                   -get how much of the song has elapsed"
        << "exit                   -exit program\n"
        << "\nDEBUG COMMANDS:\n"
        << "isplaying              -check if audio is playing\n";
}

void cmnd_load(std::string& strAudioPath, ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device, bool& decoderInitialized) {
    if (isPlaying) {
        isPlaying = false;
        deviceCleanup(decoder, device, decoderInitialized);
    }

    if (strAudioPath.empty()) {
        std::cout << "No file path provided\n";
        return;
    }

    // removes the quotation marks from the ends of the input string if any are present
    strAudioPath.erase(remove(strAudioPath.begin(), strAudioPath.end(), '\"'), strAudioPath.end());
    const char* audioPath = strAudioPath.c_str();

    result = ma_decoder_init_file(audioPath, NULL, &decoder);
    decoderInitialized = (result == MA_SUCCESS);

    if (result != MA_SUCCESS) {
        std::cout << "Could not load file: " << audioPath << "\n";
        return;
    }

    configureDevice(decoder, deviceConfig);

    std::cout << "File " << audioPath << " loaded successfuly!\n";
}

void cmnd_play(ma_result& result, ma_decoder& decoder, ma_device_config& deviceConfig, ma_device& device, bool decoderInitialized) {
    // seek to beginning
    if (isPlaying) {
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
        deviceCleanup(decoder, device, decoderInitialized);
        return;
    }

    isPlaying = true;
    std::cout << "Playing... \n";

    int seconds, minutes;
    getTime(decoder, seconds, minutes, "length");
    std::cout << "Song length: [" << std::setfill('0') << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2) << seconds << ']' << '\n';
}

void cmnd_stoppause(ma_decoder& decoder, ma_device& device, std::string cmd) {
    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
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
    if (!isPlaying) {
        std::cout << "Nothing is currently playing\n";
        return;
    }
    // check for syntax 'xx:xx'
    if (!std::regex_match(strLength, (std::regex)("^\\d{2}:\\d{2}$"))) {
        std::cout << "Invalid syntax\n";
        return;
    }

    int seconds, minutes, totalLengthSeconds;
    char colon;

    std::istringstream ss(strLength);
    ss >> minutes >> colon >> seconds;

    totalLengthSeconds = minutes * 60 + seconds;

    ma_uint64 seekFrame = static_cast<ma_uint64>(totalLengthSeconds) * decoder.outputSampleRate;;

    seekToFrame(decoder, seekFrame);
}

void cmnd_volume(ma_decoder& decoder, ma_device& device, std::string inputVolume) {
    float newVolume;
    if (!validateVolumeInput(inputVolume, newVolume)) {
        std::cout << "Please input a number from 0-100\n";
        return;
    }
    volumeMultiplier = newVolume;
}

void cmnd_elapsedTime(ma_decoder& decoder) {
    if (!isPlaying) {
        return;
    }
    
    int seconds, minutes;
    getTime(decoder, seconds, minutes, "elapsed");

    std::cout << '[' << std::setfill('0') << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2) << seconds << ']' << '\n';
}

void cmnd_readDirectory(std::string& directory) {
    std::cout << directory;
    for (const auto & entry : std::filesystem::directory_iterator(directory)) {
        auto extension = entry.path().extension();
        if (extension == ".mp3" || extension == ".wav" || extension == ".flac") {
            std::cout << entry.path() << std::endl;
        }
    }
}
