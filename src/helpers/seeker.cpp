#include "../../include/audioMaster.hpp"
#include "../../include/outputProcessor.hpp"

#include <string>
#include <sstream>
#include <regex>

ma_uint64 Seeker::frameToSeek(std::string strLength, ma_uint32 outputSampleRate) {
	int seconds, minutes, totalLengthSeconds;
	char colon;
	ma_uint64 frame;

	std::istringstream iss(strLength);
	iss >> minutes >> colon >> seconds;

	totalLengthSeconds = minutes * 60 + seconds;

	frame = static_cast<ma_uint64>(totalLengthSeconds * outputSampleRate);
	return frame;
}

void Seeker::seekFrame(ma_uint64 frame, AudioDecoder& decoder, SharedAudioState& sharedState) {
	sharedState.seeking.store(true);
	if (decoder.seek(frame) == MA_SUCCESS) {
		sharedState.currentFrame.store(frame);
	}
	sharedState.seeking.store(false);
}

bool Seeker::syntaxValid(std::string str) {
	// valid syntax is mm:ss
	std::regex validSyntax("^\\d{2}:\\d{2}$");

	if (!std::regex_match(str, validSyntax)) {
	    return false;
	}
	else return true;
}

void Seeker::seek(std::string timeToSeek, AudioDecoder& decoder, SharedAudioState& sharedState) {
	if (!sharedState.soundIsPlaying) {
	    msg("Nothing is currently playing");
	    return;
	}

	if (!syntaxValid(timeToSeek)) {
		msg("Invalid syntax");
		return;
	}

	seekFrame(frameToSeek(timeToSeek, decoder.getSampleRate()), decoder, sharedState);
}
