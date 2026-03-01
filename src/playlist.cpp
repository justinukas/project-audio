#include "../include/playlist.h"
#include "../include/playbackControl.h"
#include "../include/dataCallback.h"

#include <fstream>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

bool skipRequested = false;
std::condition_variable playNextSound;

// ==================== PLAYLIST SYSTEM ====================
// Core function
void cmnd_playlist(std::string parameter, fs::path folderPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (parameter != "make" && parameter != "play") {
		std::cout << "Invalid command. Available 'playlist' options: 'make', 'play'\n";
		return;
	}

	fs::path file("playlist.txt");	  	   // define file name
	fs::path filePath = folderPath / file; // append file to folder path

	if (parameter == "make") {
		makePlaylistFile(folderPath, filePath);
	}
	else if (parameter == "play") {
		// start seperate thread so that the user can still run commands while the playlist is running
		std::thread(playPlaylist, folderPath, std::ref(decoder), std::ref(device), std::ref(deviceConfig), std::ref(decoderInitialized)).detach();
	}
}

// Core function: make a playlist of audio files from specified directory
void makePlaylistFile(fs::path path, fs::path fullPath) {
	if (!fs::exists(path)) {
		std::cout << "Invalid path\n";
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

//Core function: play playlist
void playPlaylist(fs::path filePath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	std::map<int, std::string> playlist = readPlaylist(filePath);
	// Validate playlist
	if (playlist.empty()) {
		std::cout << "Failed to open file\n";
		return;
	}

	// loop through the whole playlist 
	for (const auto& [x, song] : playlist) {
		if (!playSingleSong(song, decoder, device, deviceConfig, decoderInitialized)) {
			break; // stop was requested
		}
	}
	postPlaybackReset(decoder, device, decoderInitialized);
}

// Helper: reads songs from file
std::map<int, std::string> readPlaylist(fs::path filePath) {
	std::ifstream in(filePath);

	std::string line;
	std::map<int, std::string> playlist;

	// return empty map if ifstream failed
	if (!in.is_open()) {
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

// Helper: plays one song and returns whether to continue. Returns false if playback should stop, true if to continue
bool playSingleSong(const std::string& song, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	std::cout << song << std::endl;
	skipRequested = false;

	cmnd_load(song, decoder, device, deviceConfig, decoderInitialized);
	cmnd_play(decoder, device, deviceConfig, decoderInitialized);

	// Wait for song to finish or skip/stop to be requested
	std::unique_lock<std::mutex> lock(audioMutex);
	playNextSound.wait(lock, [] {
		return !soundIsPlaying || skipRequested;
	});

	// return false if stop was requested (end playlist), true to continue
	return !stopRequested;
}

// Helper: resets playback after playlist ends
void postPlaybackReset(ma_decoder& decoder, ma_device& device, ma_result& decoderInitialized) {
	// clean up post playback (in case it wasn't cleaned via stop)
	if (!stopRequested) {
		cleanup(decoder, device, decoderInitialized);
	}

	stopRequested = false;
	std::cout << "DEBUG: exiting playlist thread\n";
}

// Helper: skips a playlist song
void cmnd_skip(ma_decoder& decoder) {
	skipRequested = true;
	playNextSound.notify_one();
}
