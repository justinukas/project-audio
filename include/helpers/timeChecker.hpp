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
	Time timeByType(std::string type, AudioDecoder& decoder);
public:
	void outputTime(std::string type, AudioDecoder& decoder, SharedAudioState& sharedState);
};
