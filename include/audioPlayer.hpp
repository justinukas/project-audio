#pragma once

#include "audioSetup.hpp"
#include "seeker.hpp"
#include "dataCallback.hpp"
#include "timeChecker.hpp"
#include "volumeControl.hpp"
#include "playlistManager.hpp"
#include "outputProcessor.hpp"

#include <string>
#include <filesystem>
#include <atomic>
#include <mutex>

namespace fs = std::filesystem;

class AudioPlayer {
private:
	AudioDecoder decoder;
	AudioDevice device;
    Seeker seeker;
    TimeChecker timeChecker;
    VolumeControl volumeControl;
    PlaylistManager playlistManager;

    bool paused = false;
    std::atomic<bool> stopRequested;
    std::atomic<bool> skipRequested;

    ma_device_config configuration() {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = decoder.getFormat();
        config.playback.channels = decoder.getChannels();
        config.sampleRate = decoder.getSampleRate();
        config.dataCallback = dataCallback;
        config.pUserData = this;

        return config;
    }

    void cleanup() {
        std::lock_guard<std::mutex> lock(audioMutex);
  	    if (device.isStarted()) {
        	device.stop();
  	    }
  	    device.uninit();
  	    decoder.uninit();
  	    decoder.clearResult();

  	    soundIsPlaying = false;
        playbackFinished = true;
  	    msg("DEBUG: Cleaned up");
    }

    bool playPlaylistSong(const std::string& song);

public:    
	bool initializeFile(std::string songPath);
    bool play();

    void stop() {
        if (!soundIsPlaying) {
            msg("Nothing is playing");
		    return;
	    }
	    if (decoder.getResult() != MA_ERROR) {
		    cleanup();
	    }

	    stopRequested = true;
        msg("Stopped playback");
    }
    void pause() {
        if (!soundIsPlaying) {
            msg("Nothing is playing");
		    return;
	    }
	    device.stop();
	    paused = true;
        msg("Paused playback");
    }

    void seek(std::string timeToSeek) { seeker.seek(timeToSeek, decoder); }
    void getElapsedTime() { timeChecker.outputTime("elapsed", decoder); }
    void getSongLength() { timeChecker.outputTime("length", decoder); }
    void setVolume(std::string inputVolume) { volumeControl.setVolume(inputVolume); }

    void playPlaylist(std::string filePath);
    void makePlaylist(std::string folderPath) { 
        fs::path file = "playlist.txt";
        fs::path filePath = folderPath / file;
        playlistManager.makePlaylistFile(folderPath, filePath);
    }
    void skipPlaylistSong() {
        if (!soundIsPlaying) {
            msg("Nothing to skip");
            return;
        }
        skipRequested = true; 
    }
};
