#pragma once

#include "globalVars.hpp"
#include "audioSetup.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <regex>

class Seeker {
private:
    ma_uint64 frameToSeek(std::string strLength, ma_uint32 outputSampleRate) {
	    int seconds, minutes, totalLengthSeconds;
	    char colon;
	    ma_uint64 frame;

	    std::istringstream iss(strLength);
	    iss >> minutes >> colon >> seconds;

	    totalLengthSeconds = minutes * 60 + seconds;

	    frame = static_cast<ma_uint64>(totalLengthSeconds * outputSampleRate);
	    return frame;
    }

    void seekFrame(AudioDecoder& decoder, ma_uint64 frame) {
        std::lock_guard<std::mutex> lock(audioMutex);
        decoder.seek(frame);
    }

public:
    void seek(std::string timeToSeek, AudioDecoder& decoder) {
	    if (!soundIsPlaying) {
	    	std::cout << "Nothing is currently playing\n";
	    	return;
	    }

	    // valid syntax is mm:ss
	    std::regex validSyntax("^\\d{2}:\\d{2}$");

	    if (!std::regex_match(timeToSeek, validSyntax)) {
	    	std::cout << "Invalid syntax\n";
	    	return;
	    }

	    seekFrame(decoder, frameToSeek(timeToSeek, decoder.getSampleRate()));
    }
};
