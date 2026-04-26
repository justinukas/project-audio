#pragma once

#include "miniaudio.h"
#include "audioSetup.hpp"
#include "globalVars.hpp"
#include "audioSetup.hpp"
#include "outputProcessor.hpp"

#include <string>
#include <iomanip>
#include <iostream>

struct Time {
	int minutes;
	int seconds;
};

class TimeChecker {
private:
	Time timeByType(std::string type, AudioDecoder decoder) {
		std::lock_guard<std::mutex> lock(audioMutex);
		int minutes = 0, seconds = 0;

		ma_uint64 frames = 0;
		if (type == "elapsed") {
   	    	decoder.getElapsedFrames(&frames);
		}
		else if (type == "length") {
		    decoder.getAudioLength(&frames);
		}

		// convert frames to seconds
		if (decoder.getSampleRate() != 0) {
			seconds = static_cast<int>(frames / decoder.getSampleRate());
		}

		minutes = seconds / 60;
		seconds -= minutes * 60;
		return { minutes, seconds };
	}
public:
	void outputTime(std::string type, AudioDecoder decoder) {
		if (!soundIsPlaying) {
			return;
		}

		Time timeToOutput = timeByType(type, decoder);
		std::ostringstream oss;
		if (type == "length") {
			oss << "Song length: ";
		}
		oss << '[' << std::setfill('0') << std::setw(2) << timeToOutput.minutes << ':' << std::setfill('0') << std::setw(2) << timeToOutput.seconds << ']' << '\n';
		msg(oss.str());
	}
};
