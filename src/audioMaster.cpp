#include "../include/audioMaster.hpp"

AudioMaster::AudioMaster() : device(this) {}

void AudioMaster::cleanup() {
        //std::lock_guard<std::mutex> lock(audioMutex);
  	    if (device.isStarted()) {
        	device.stop();
  	    }
  	    device.uninit();
  	    decoder.uninit();
  	    decoder.clearResult();

  	    audioState.soundIsPlaying.store(false);
        audioState.playbackFinished.store(true);
  	    msg("DEBUG: Cleaned up");
    }
bool AudioMaster::initializeDevice() { return device.initialize(decoder); }
