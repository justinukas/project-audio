#include "../include/audioPlayer.hpp"
#include "../include/outputProcessor.hpp"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

bool AudioPlayer::initializeFile(std::string songPath) {
  	// i forgor why do this and not just cleanup
  	// because if device is started, chances are, result is MA_SUCCESS, so
  	// everything gets cleaned up either way
  	if (device.isStarted()) {
        device.uninit();
        soundIsPlaying = false;
  	}

  	if (songPath.empty()) {
        msg("No file path provided");
        return false;
  	}

	if (decoder.getResult() == MA_SUCCESS && !cleanedUp) {
       	cleanup();
    }

    fs::path path(songPath);
    if (!fs::exists(songPath)) {
	  	msg("Path does not exist");
       	return false;
    }

    if (!fs::is_regular_file(path)) {
       	msg("Path is not a file");
       	return false;
    }

    const char* audioPath = songPath.c_str();
    decoder.setResult(decoder.initFile(audioPath));

    if (decoder.getResult() != MA_SUCCESS) {
		std::ostringstream oss;
		oss << "Could not load file: \"" << audioPath << '\"';
		msg(oss.str());
       	return false;
    }

	cleanedUp = false;
	std::ostringstream oss;
	oss << "File " << audioPath << " loaded succesfully!";
	msg(oss.str());

	return true;
}

bool AudioPlayer::play() {
	if (decoder.getResult() != MA_SUCCESS) {
		msg("Please load a file using 'load <file_path>' first!");
		return false;
	}

	if (!soundIsPlaying && playbackFinished) {
		msg("Please load a new file");
		return false;
	}

	// resume if paused
	if (paused) {
		device.start();
		paused = false;
		return false;
	}

	const std::string audioStartTime = "00:00";
	// start from the beginning if something is already playing
	if (soundIsPlaying) {
		seeker.seek(audioStartTime, decoder);
		return false;
	}

    if (!device.initialize(configuration())) {
        msg("Failed to open playback device");
		decoder.uninit();
		return false;
    }

    if (!device.start()) {
        msg("Failed to start playback device");
		device.uninit();
		decoder.uninit();
		return false;
    }

	soundIsPlaying = true;

	Time length = timeChecker.getTime("length", decoder);
	std::ostringstream oss;
	oss << "Playing..." << '\n'
		<< "Song length: "
		<< '[' << std::setfill('0') << std::setw(2) << length.minutes << ':' << std::setfill('0') << std::setw(2) << length.seconds << ']';
	msg(oss.str());

	return true;
}
