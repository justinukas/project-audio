#pragma once

#include "audioSetup.hpp"
#include "helpers/audioPlayer.hpp"
#include "helpers/seeker.hpp"
#include "helpers/timeChecker.hpp"
#include "helpers/volumeController.hpp"
#include "helpers/playlistMaster.hpp"
#include "outputProcessor.hpp"
#include <atomic>

struct SharedAudioState {
    // states for everything
    std::atomic<double> volumeMultiplier;
    std::atomic<bool> soundIsPlaying;
    std::atomic<bool> playbackFinished;
    bool paused = false;

    // states for playlists/queues
    std::atomic<bool> stopRequested;
    std::atomic<bool> skipRequested;
};

class AudioMaster {
    friend class DataCallback;
private:
    AudioDecoder decoder;
    AudioDevice device;
    SharedAudioState audioState;
    //AudioPlayer player;
    //Seeker seeker;
    //TimeChecker timeChecker;
    //VolumeController volumeController;
    //PlaylistMaster playlistMaster;
public:
    AudioMaster() : device(this){}

    void cleanup() {
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
    bool initializeDevice() { return device.initialize(); }

    SharedAudioState* statePointer() { return &audioState; }
};
