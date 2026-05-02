#pragma once

#include "../outputProcessor.hpp"
#include "../../libraries/miniaudio.h"

#include <string>

class AudioDecoder;
class SharedAudioState;

class Seeker {
private:
    ma_uint64 frameToSeek(std::string strLength, ma_uint32 outputSampleRate);
    void seekFrame(AudioDecoder& decoder, ma_uint64 frame);
	bool syntaxValid(std::string str);

public:
    void seek(std::string timeToSeek, AudioDecoder& decoder, SharedAudioState& sharedState);
};
