#include "../../include/helpers/queueMaster.hpp"
#include "../../libraries/miniaudio.h"
#include "../../include/outputProcessor.hpp"
#include "../../include/audioMaster.hpp"
#include <sstream>
#include <algorithm>
#include <thread>

namespace fs = std::filesystem;

void QueueMaster::addToQueue(const fs::path& songToAdd) {
	if(!fs::exists(songToAdd)) {
		msg("File does not exist");
		return;
	}

	// check if file extension is one of the supported ones
	std::vector<fs::path> supportedExtensions = { ".mp3", ".wav", ".flac" }; // supported extensions
	auto fileExtension = songToAdd.extension();

	if (std::find(supportedExtensions.begin(), supportedExtensions.end(), fileExtension) == supportedExtensions.end()) {
		msg("Unsupported file format");
		return;
	}

	songQueue.push_back(songToAdd);
	msg("Added song " + songToAdd.string());
}

void QueueMaster::removeFromQueue(const int& index) {
	if (index > songQueue.size() || index < 0) {
		msg("No song exists at this position");
		return;
	}
	// might need to add -1 to this
    songQueue.erase(songQueue.begin()+index);
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


	// return false if stop was requested (end playlist), true to continue
	return !sharedState.stopRequested.load();
}

void QueueMaster::playQueue(AudioMaster& master, SharedAudioState& sharedState, AudioDecoder& decoder) {
    // Validate playlist
    if (songQueue.empty()) {
    	msg("Queue is empty");
    	return;
	}

	// loop through the whole playlist
    for (currentSong = 0; currentSong < songQueue.size(); currentSong++) {
		if (!playQueueSong(songQueue[currentSong], master, sharedState)) {
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

    // -1 from currentSong 
    // skipCurrent = true
void QueueMaster::playPreviousTrack() {
	if (currentSong - 1 < 0) {
		msg("No previous song");
		return;
	}
	// -2 because when the loop continues its gonna currentSong++
	currentSong -= 2;
	skipCurrent.store(true);
}

void QueueMaster::playNextTrack(SharedAudioState& sharedState) {
    if (!sharedState.soundIsPlaying.load()) {
        msg("Nothing to skip");
        return;
    }
    skipCurrent.store(true);
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
