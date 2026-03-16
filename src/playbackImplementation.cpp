#include "../include/audioPlayer.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

void AudioPlayer::initializeFile(std::string songPath) {
  	// i forgor why do this and not just cleanup
  	// because if device is started, chances are, result is MA_SUCCESS, so
  	// everything gets cleaned up either way
  	if (device.isStarted()) {
        device.uninit();
        soundIsPlaying = false;
  	}

  	if (songPath.empty()) {
        std::cout << "No file path provided\n";
        return;
  	}

	if (decoder.getResult() == MA_SUCCESS) {
       	cleanup();
    }

    fs::path path(songPath);
    if (!fs::exists(songPath)) {
	  	std::cout << "Path does not exist\n";
       	return;
    }

    if (!fs::is_regular_file(path)) {
       	std::cout << "Path is not an audio file\n";
       	return;
    }

    const char* audioPath = songPath.c_str();
    decoder.setResult(decoder.initFile(audioPath));

    if (decoder.getResult() != MA_SUCCESS) {
      	std::cout << "Could not load file: \"" << audioPath << "\"\n";
       	return;
    }

    std::cout << "File " << audioPath << " loaded successfuly!\n";
}

#include <thread>

void AudioPlayer::play() {
	std::cout << "play id:" << std::this_thread::get_id() << std::endl;
	if (decoder.getResult() != MA_SUCCESS) {
		std::cout << "Please load a file using 'load <file_path>' first!\n";
		return;
	}

	if (!soundIsPlaying && playbackFinished) {
		std::cout << "Please load a new file\n";
		return;
	}

	// resume if paused
	if (paused) {
		device.start();
		paused = false;
		return;
	}

	const std::string audioStartTime = "00:00";
	// start from the beginning if something is already playing
	if (soundIsPlaying) {
		seeker.seek(audioStartTime, decoder);
		return;
	}

    if (!device.initialize(configuration())) {
        std::cout << "Failed to open playback device\n";
		decoder.uninit();
		return;
    }

    if (!device.start()) {
        std::cout << "Failed to start playback device\n";
		device.uninit();
		decoder.uninit();
		return;
    }

	soundIsPlaying = true;

	Time length = timeChecker.getTime("length", decoder);
	std::cout << "Playing..." << '\n'
		<< "Song length: "
		<< '[' << std::setfill('0') << std::setw(2) << length.minutes << ':' << std::setfill('0') << std::setw(2) << length.seconds << ']' << '\n';
}
