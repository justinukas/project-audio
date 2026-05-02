#include "../../libraries/miniaudio.h"
#include "../../include/helpers/timeChecker.hpp"
#include "../../include/audioMaster.hpp"
#include "../../include/outputProcessor.hpp"

#include <string>
#include <iomanip>

Time TimeChecker::timeByType(std::string type, AudioDecoder& decoder) {
	//std::lock_guard<std::mutex> lock(audioMutex);
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

void TimeChecker::outputTime(std::string type, AudioDecoder& decoder, SharedAudioState& sharedState) {
	if (!sharedState.soundIsPlaying) {
		msg("Nothing is currently playing");
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
