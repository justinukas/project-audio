#include "../../include/helpers/queueMaster.hpp"
#include "../../libraries/miniaudio.h"
#include "../../include/outputProcessor.hpp"
#include "../../include/audioMaster.hpp"
#include <sstream>
#include <algorithm>
#include <thread>
#include <random>
#include <fstream>

namespace fs = std::filesystem;

void QueueMaster::addToQueue(const fs::path& songToAdd) {
	if(!fs::exists(songToAdd)) {
		msg("File does not exist");
		return;
	}

	// check if file extension is one of the supported ones
    std::vector<fs::path> supportedExtensions = { ".mp3", ".wav", ".flac" }; // supported file extensions
	auto fileExtension = songToAdd.extension();

	if (std::find(supportedExtensions.begin(), supportedExtensions.end(), fileExtension) == supportedExtensions.end()) {
		msg("Unsupported file format");
		return;
	}

	songQueue.push_back(songToAdd);
	msg("Added song " + songToAdd.string());
}

void QueueMaster::removeFromQueue(const int& position) {
	if (position > songQueue.size() || position < 0) {
		msg("No song exists at specified position");
		return;
	}
	
	if (position == currentPosition) {
		currentPosition--;
		skipCurrent.store(true);
	}
    songQueue.erase(songQueue.begin() + position);
}

void QueueMaster::shuffleQueue() {
	if (songQueue.empty()) {
		msg("Queue is empty");
		return;
	}

	if (!isShuffled) {
		queueBackup = songQueue;

		auto rng = std::default_random_engine {};
		std::shuffle(songQueue.begin(), songQueue.end(), rng);
		msg("Queue has been shuffled");
		listQueue();
	}
	else if (isShuffled) {
		songQueue = queueBackup;
		queueBackup.erase(queueBackup.begin(), queueBackup.end());

		msg("Queue has been unshuffled");
	}

	isShuffled = !isShuffled;

	// -1 because its gonna +1 when the next loop starts
	currentPosition = -1;
	skipCurrent.store(true);
}

void QueueMaster::moveSong(const int& oldPosition, const int& newPosition) {
	if (songQueue.empty()) {
		msg("Queue is empty");
		return;
	}
	if (songQueue.size() < oldPosition) {
		msg("No song exists at picked song's position");
		return;
	}
	if (songQueue.size() < newPosition) {
		msg("Specified new position is larger than queue");
		return;
	}
	if (newPosition < 0) {
		msg("Specified new position is less than 0");
		return;
	}

	fs::path songCopy = songQueue[oldPosition];
	songQueue.erase(songQueue.begin() + oldPosition);
	songQueue.insert(songQueue.begin() + newPosition, songCopy);

	if (oldPosition == currentPosition) {
		currentPosition = newPosition;
	}
	msg("Song moved to position " + std::to_string(newPosition));
}

void QueueMaster::listQueue() {
	if (songQueue.empty()) {
		msg("Queue is empty");
		return;
	}

	std::ostringstream oss;
	oss << "Songs in the queue: " << std::endl;
	for (int i = 0; i < songQueue.size(); i++) {
		// to avoid linebreak after the list is done
		if (i == songQueue.size()-1) {
			oss << i << ' ' << songQueue[i];
			
		}
		else oss << i << ' ' << songQueue[i] << std::endl;
	}
	msg(oss.str());
}

void QueueMaster::saveAsFile(const fs::path& destination) {
	if (songQueue.empty()) {
		msg("Queue is empty");
		return;
	}
	if (!fs::exists(destination)) {
		msg("Invalid destination");
		return;
	}

	fs::path fileName = "savedQueue.txt";
	std::ofstream out(destination / fileName);
	for (int i = 0; i < songQueue.size(); i++) {
		out << i << ' ' << songQueue[i] << std::endl;
	}
}

#include <iostream>

void QueueMaster::readFromFile(const fs::path& path) {
	std::ifstream in(path);

	// return empty map if ifstream failed
	if (!in.is_open()) {
		msg("File reading failed");
		return;
	}

	if (path.extension() != ".txt") {
		msg("File is not a text file");
		return ;
	}

	std::string line;
	std::vector<fs::path> readQueue;

	while (std::getline(in, line))
	{
		std::string number;
		std::string songPath;

		std::istringstream ss(line);
		std::getline(ss, number, ' ');
		std::getline(ss, songPath);

		// strip quotes/apostrophes at ends of path
		if ((songPath.size() >= 2) && 
		   ((songPath.front() == '"' && songPath.back() == '"') || 
		    (songPath.front() == '\'' && songPath.back() == '\''))) {

		    songPath = songPath.substr(1, songPath.size() - 2);
        }

		readQueue.push_back(songPath);
	}
	songQueue = readQueue;
}

bool QueueMaster::playQueueSong(const fs::path& song, AudioMaster& master, const SharedAudioState& sharedState) {
    skipCurrent.store(false);

	// skip song if play or load fail
	if(!master.initializeFile(song)) {
		return true;
	}
	if(!master.playSong()) {
		return true;
	}

	// Wait for song to finish or skip/stop to be requested
    while(sharedState.soundIsPlaying.load() && !skipCurrent.load() && !sharedState.stopRequested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	if (sharedState.stopRequested.load()) msg("oh im stopping all over the place");
	if (skipCurrent.load()) msg("oh im skipping it and shit");


	// return false if stop was requested (end queue playback), true to continue
	return !sharedState.stopRequested.load();
}

void QueueMaster::playQueue(AudioMaster& master, SharedAudioState& sharedState, AudioDecoder& decoder) {
    // Validate queue
    if (songQueue.empty()) {
    	msg("Queue is empty");
    	return;
	}

	// loop through the whole queue
    for (currentPosition = 0; currentPosition < songQueue.size(); currentPosition++) {
		if (!playQueueSong(songQueue[currentPosition], master, sharedState)) {
		    break; // stop was requested
		}
	}

	// clean up post playback (in case it wasn't cleaned via stop)
	if (decoder.getResult() == MA_SUCCESS) {
		// might crash here if there was an error encountered in load or play stages
		master.cleanup();
	}

    sharedState.stopRequested.store(false);;

	songQueue.erase(songQueue.begin(), songQueue.end());
    msg("Queue has finished");
}

void QueueMaster::playPreviousTrack() {
	if (currentPosition - 1 < 0) {
		msg("No previous song");
		return;
	}
	// -2 because when the loop continues its gonna currentSong++
	currentPosition -= 2;
	skipCurrent.store(true);
}

void QueueMaster::playNextTrack(SharedAudioState& sharedState) {
    if (!sharedState.soundIsPlaying.load()) {
        msg("Nothing to skip");
        return;
    }
    skipCurrent.store(true);
}

void QueueMaster::selectQueueSong(const int& position) {
	if (songQueue.empty()) {
		msg("Queue is empty");
		return;
	}

	if (position > songQueue.size() || position < 0) {
		msg("No song exists at specified position");
		return;
	}

	// -1 because its gonna +1 when the next loop starts
	currentPosition = position - 1;
	skipCurrent.store(true);
}
