#include "../../libraries/miniaudio.h"
#include "../../include/helpers/timeChecker.hpp"
#include "../../include/audioMaster.hpp"
#include "../../include/outputProcessor.hpp"

#include <string>
#include <iomanip>

Time TimeChecker::timeByType(const std::string& type, AudioDecoder& decoder, const SharedAudioState& sharedState) {
	int minutes = 0, seconds = 0;

	ma_uint64 frames = 0;
	if (type == "elapsed") {
   	    frames = sharedState.currentFrame.load();
	}
	else if (type == "length") {
		frames = sharedState.totalFrames.load();
	}

	// convert frames to seconds
	if (decoder.getSampleRate() != 0) {
		seconds = static_cast<int>(frames / decoder.getSampleRate());
	}

	minutes = seconds / 60;
	seconds -= minutes * 60;
	return { minutes, seconds };
}

void TimeChecker::outputTime(const std::string& type, AudioDecoder& decoder, const SharedAudioState& sharedState) {
	if (!sharedState.soundIsPlaying) {
		msg("Nothing is currently playing");
		return;
	}

	Time timeToOutput = timeByType(type, decoder, sharedState);
	std::ostringstream oss;
	if (type == "length") {
		oss << "Song length: ";
	}
	else if (type == "elapsed") {
		oss << "Elapsed: ";
	}
	oss << '[' << std::setfill('0') << std::setw(2) << timeToOutput.minutes << ':' << std::setfill('0') << std::setw(2) << timeToOutput.seconds << ']';
	msg(oss.str());
}
