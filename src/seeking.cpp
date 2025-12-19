#include "../include/seeking.h"
#include "../include/dataCallback.h"

#include <regex>
#include <iostream>
#include <sstream>

void seekFrame(ma_decoder& decoder, ma_uint64 frame) {
    std::lock_guard<std::mutex> lock(audioMutex);
    ma_decoder_seek_to_pcm_frame(&decoder, frame);
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
