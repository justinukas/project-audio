#include "../include/playlist.h"
#include "../include/playbackControl.h"
#include "../include/dataCallback.h"
//#include "../include/seeking.h"
//#include "../include/time.h"

#include <fstream>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

bool skipRequested = false;
std::condition_variable playNextSound;

void static makePlaylistFile(fs::path path, fs::path fullPath) {
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

std::map<int, std::string> static readPlaylist(fs::path filePath) {
	std::ifstream in(filePath);

	std::string line;
	std::map<int, std::string> playlist;

	if (!in.is_open()) {
		return {};	// return empty map
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

void static playPlaylist(fs::path filePath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	std::map<int, std::string> playlist = readPlaylist(filePath);
	if (playlist.empty()) {
		std::cout << "Failed to open file\n";
		return;
	}

	// loop through the whole playlist 
	for (const auto& item : playlist) {
		std::string song = item.second;
		std::cout << song << std::endl;

		cmnd_load(song, decoder, device, deviceConfig, decoderInitialized);
		cmnd_play(decoder, device, deviceConfig, decoderInitialized);

		// "unrequest" skip after skipping
		skipRequested = false;

		std::unique_lock<std::mutex> lock(audioMutex);

		// pauses playlist loop while:
		// 1. sound is playing
		// 2. skip isn't requested
		playNextSound.wait(lock, [] {
			return !soundIsPlaying || skipRequested;
			});

		// stop playlist playback if stop is requested
		if (stopRequested) { break; }
	}

	// cleans up playback when you skip the last song (when you do, it continues playing since skip doesnt have a cleanup function)
	// it only cleans up when stop isn't requested because stop already calls the cleanup function
	if (!stopRequested) {
		cleanup(device, decoder, decoderInitialized);
	}

	// "unrequest" stop after playlist finishes
	stopRequested = false;
	std::cout << "DEBUG: exiting playlist thread\n";
}

void cmnd_playlist(std::string parameter, fs::path folderPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (parameter != "make" && parameter != "play") {
		std::cout << "Invalid command. Available 'playlist' options: 'make', 'play'\n";
		return;
	}

	fs::path file("playlist.txt");	  // define file name
	fs::path filePath = folderPath / file;

	if (parameter == "make") {
		makePlaylistFile(folderPath, filePath);
	}
	else if (parameter == "play") {
		// start seperate thread so that the user can still run commands while the playlist is running
		std::thread(playPlaylist, folderPath, std::ref(decoder), std::ref(device), std::ref(deviceConfig), std::ref(decoderInitialized)).detach();
	}
}

// function for skipping a playlist song
void cmnd_skip(ma_decoder& decoder) {
	skipRequested = true;
	playNextSound.notify_one();
}
