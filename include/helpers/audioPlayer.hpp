#pragma once

#include <string>
#include <functional>

#include "seeker.hpp"

class AudioDecoder;
class AudioDevice;
class SharedAudioState;
class TimeChecker;

class AudioPlayer {
private:
	bool paused = false;

public:
	bool initializeFile(std::string songPath, AudioDecoder& decoder, AudioDevice& device, std::function<void()> cleanup);
    bool play(AudioDecoder& decoder, AudioDevice& device, SharedAudioState& sharedState, std::function<bool()> initializeDevice, Seeker seeker, TimeChecker timeChecker);
    void pause(AudioDevice& device, SharedAudioState& sharedState);
	void stop(AudioDecoder& decoder, SharedAudioState& sharedState, std::function<void()> cleanup);
};
