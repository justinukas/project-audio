#include "../include/audioPlayer.hpp"
#include "../include/outputProcessor.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

bool AudioPlayer::initializeFile(std::string songPath) {
  	if (songPath.empty()) {
        msg("No file path provided");
        return false;
  	}

	// if not clean
	if (decoder.getResult() == MA_SUCCESS) {
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

	//AudioPlayer::play(): * Calls Device.init(decoder, this).

	//Inside that init, the Device calls its own configuration() helper using the decoder's info.

	//Calls Device.start().
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

	msg("Playing...");
	timeChecker.outputTime("length", decoder);

	return true;
}
