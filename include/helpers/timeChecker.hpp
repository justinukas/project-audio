#pragma once

#include "../../libraries/miniaudio.h"
#include "../outputProcessor.hpp"

#include <string>

class AudioDecoder;
class SharedAudioState;

struct Time {
	int minutes;
	int seconds;
};

class TimeChecker {
private:
	Time timeByType(const std::string& type, AudioDecoder& decoder, const SharedAudioState& sharedState);
public:
	void outputTime(const std::string& type, AudioDecoder& decoder, const SharedAudioState& sharedState);
};
