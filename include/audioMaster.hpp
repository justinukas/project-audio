#pragma once

#include "audioSetup.hpp"
#include "helpers/audioPlayer.hpp"
#include "helpers/seeker.hpp"
#include "helpers/timeChecker.hpp"
#include "helpers/volumeController.hpp"
#include "helpers/playlistMaster.hpp"
#include "helpers/queueMaster.hpp"
#include "outputProcessor.hpp"
#include <atomic>

struct SharedAudioState {
    // states for everything
    std::atomic<double> volumeMultiplier{0.5};
    std::atomic<bool> soundIsPlaying;
    std::atomic<bool> playbackFinished;

    // frame related
    std::atomic<ma_uint64> currentFrame{0};
    std::atomic<ma_uint64> totalFrames{0};
    std::atomic<bool> seeking{false};

    // states for playlists/queues
    // maybe split this off down the line
    std::atomic<bool> queueMode{false};
    std::atomic<bool> stopRequested{false};
};

class AudioDevice;
class AudioDecoder;

class AudioMaster {
    friend class DataCallback;
private:
    AudioDecoder decoder;
    AudioDevice device;
    SharedAudioState sharedState;
    AudioPlayer player;
    Seeker seeker;
    TimeChecker timeChecker;
    VolumeController volumeController;

    QueueMaster queueMaster;
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

  	    sharedState.soundIsPlaying.store(false);
        sharedState.playbackFinished.store(true);
  	    msg("DEBUG: Cleaned up");
    }
    bool initializeDevice() { return device.initialize(decoder); }

    SharedAudioState* statePointer() { return &sharedState; }

    // BASIC FUNCTIONS
    bool initializeFile(const fs::path& songPath) { return player.initializeFile(songPath, decoder, device, [this]() { cleanup(); }, sharedState); }
    bool playSong() { return player.play(decoder, device, sharedState, [this]() { return initializeDevice(); }, seeker, timeChecker); }
    void pausePlayback() { player.pause(device, sharedState); }
    void stopPlayback() { player.stop(decoder, sharedState, [this]() { cleanup(); }); }
    void seek(std::string userInput) { seeker.seek(userInput, decoder, sharedState); }
    void getTime(std::string timeType) { timeChecker.outputTime(timeType, decoder, sharedState); }
    void setVolume(std::string userInput) { volumeController.setVolume(userInput, sharedState); }


    // QUEUE FUNCTIONS
    void addToQueue(const fs::path& songPath) { queueMaster.addToQueue(songPath); }
    void removeFromQueue(const int& index) { queueMaster.removeFromQueue(index); }
    void playQueue() { queueMaster.playQueue(*this, sharedState, decoder); }
    void listQueue() { queueMaster.listQueue(); }
    void nextSong() { queueMaster.playNextTrack(sharedState); }
    void previousSong() { queueMaster.playPreviousTrack(); }
};
