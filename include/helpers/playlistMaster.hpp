/*#pragma once
#include "outputProcessor.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

class PlaylistMaster {
private:
    // Reads songs from file
    std::map<int, std::string> playlist(fs::path filePath) {
	    std::ifstream in(filePath);

	    std::string line;
	    std::map<int, std::string> playlist;

	    // return empty map if ifstream failed
	    if (!in.is_open()) {
			msg("File reading failed");
		    return {};
	    }

		if (filePath.extension() != ".txt") {
			msg("File is not a text file");
			return {};
		}

	    while (std::getline(in, line))
	    {
		    std::string number;
		    std::string path;

		    std::istringstream ss(line);
		    std::getline(ss, number, ' ');
		    std::getline(ss, path);

		    playlist.insert({ stoi(number), path });
	    }
	    return playlist;
    }
	bool playPlaylistSong(const std::string& song) {
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

public:
    // core function: make a playlist of audio files from specified directory
    void makePlaylistFile(fs::path path, fs::path fullPath) {
	    if (!fs::exists(path)) {
	    	msg("Invalid path");
	    	return;
	    }

	    std::vector<fs::path> extensions = { ".mp3", ".wav", ".flac" }; // supported extensions
	
	    std::ofstream out(fullPath);
    	int playlistPos = 1;	// position of song in a playlist

    	fs::directory_iterator playlistDirectory(path);

    	// read through the user defined directory
    	for (const auto& directoryEntry : playlistDirectory) {
	    	auto fileExtension = directoryEntry.path().extension();

	    	// check if file extension is one of the supported ones
	    	if (find(extensions.begin(), extensions.end(), fileExtension) != extensions.end()) {
	    		out << playlistPos << ' ' << directoryEntry.path().string() << std::endl;
	    		playlistPos++;
	    	}
	    }
    }

	void playPlaylist(fs::path filePath) {
		std::map<int, std::string> playlist = playlist(filePath);
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
*/
