#pragma once

#include "audioSetup.hpp"
#include "dataCallback.hpp"
#include "seeker.hpp"
#include "timeChecker.hpp"
#include "volumeControl.hpp"
#include "playlistManager.hpp"
#include "globalVars.hpp"

#include <string>
#include <iostream>

class AudioPlayer {
private:
	AudioDecoder decoder;
	AudioDevice device;
    Seeker seeker;
    TimeChecker timeChecker;
    VolumeControl volumeControl;
    PlaylistManager playlistManager;

    bool paused = false;

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

    void playPlaylist(fs::path filePath);
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

    void playlistMakerPlayer(std::string parameter, fs::path folderPath);
    void skipPlaylistSong() {
        if (!soundIsPlaying) {
            std::cout << "Nothing to skip\n";
            return;
        }
        skipRequested = true; 
    }
};
