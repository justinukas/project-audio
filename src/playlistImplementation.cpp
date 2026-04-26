#include "../include/audioPlayer.hpp"
#include "../include/outputProcessor.hpp"

#include <thread>

void AudioPlayer::playPlaylist(std::string filePath) {
    std::map<int, std::string> playlist = playlistManager.playlist(filePath);
    // Validate playlist
    if (playlist.empty()) {
    	msg("Failed to open file");
    	return;
	}

    // loop through the whole playlist 
    for (const auto& [x, song] : playlist) {
	    if (!playPlaylistSong(song)) {
	    	break; // stop was requested
	    }
	}

	// clean up post playback (in case it wasn't cleaned via stop)
	if (decoder.getResult() == MA_SUCCESS) {
		// crashes here if there was an error encountered in load or play stages
	    cleanup();
	}

    stopRequested = false;
    msg("DEBUG: exiting playlist thread");
}

// Plays one song and returns whether to continue. Returns false if playback should stop, true if to continue
bool AudioPlayer::playPlaylistSong(const std::string& song) {
    skipRequested = false;

	// skip song if play or load fail
	if(!initializeFile(song)) {
		return true;
	}
	if(!play()) {
		return true;
	}

	// Wait for song to finish or skip/stop to be requested
    while(soundIsPlaying && !skipRequested && !stopRequested) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	// return false if stop was requested (end playlist), true to continue
	return !stopRequested;
}
