#pragma once

#include "audioSetup.hpp"
#include "seeker.hpp"
#include "dataCallback.hpp"
#include "timeChecker.hpp"
#include "volumeControl.hpp"
#include "playlistManager.hpp"

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
        config.pUserData = &decoder;

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
  	    std::cout << "Cleaned up\n";
    }

    bool playPlaylistSong(const std::string& song);

public:    
	void initializeFile(std::string songPath);
    void play();

    void stop() {
        if (!soundIsPlaying) {
            std::cout << "Nothing is playing\n";
		    return;
	    }
	    if (decoder.getResult() != MA_ERROR) {
		    cleanup();
	    }

	    //stopRequested = true
        std::cout << "Stopped playback\n";
    }
    void pause() {
        if (!soundIsPlaying) {
            std::cout << "Nothing is playing\n";
		    return;
	    }
	    device.stop();
	    paused = true;
        std::cout << "Paused playback\n";
    }

    void seek(std::string timeToSeek) { seeker.seek(timeToSeek, decoder); }
    void getElapsedTime() { timeChecker.getElapsedTime(decoder); }
    void setVolume(std::string inputVolume) { volumeControl.setVolume(inputVolume); }

    void playPlaylist(fs::path filePath);
    void makePlaylist(fs::path folderPath) { 
        fs::path file = "playlist.txt";
        fs::path filePath = folderPath / file;
        playlistManager.makePlaylistFile(folderPath, filePath);
    }
    void skipPlaylistSong() {
        if (!soundIsPlaying) {
            std::cout << "Nothing to skip\n";
            return;
        }
        skipRequested = true; 
    }
};
