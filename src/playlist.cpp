#include "../include/playlist.h"
#include "../include/playbackControl.h" // cmnd_{load, play}(), stopRequested
#include "../include/dataCallback.h"    // soundIsPlaying
#include "../include/seeking.h"         // seekToFrame
#include "../include/time.h"            // Time, getLength()

#include <map>
#include <fstream>
#include <thread>
#include <filesystem>
#include <iostream>
namespace fs = std::filesystem;

bool skipRequested = false;

void static makePlaylistFile(fs::path path, fs::path fullPath) {
	if (!fs::exists(path)) {
		std::cout << "Invalid path\n";
		return;
	}

	std::vector<fs::path> extensions = { ".mp3", ".wav", ".flac" }; // supported extensions
	
	std::ofstream out(fullPath);
	int i = 1;

	// read through the user defined directory
	for (const auto& entry : fs::directory_iterator(path)) {
		auto extension = entry.path().extension();

		// check if file extension is one of the supported ones
		if (find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
			out << i << ' ' << entry.path().string() << std::endl;
			i++;
		}
	}
}

std::map<int, std::string> static readPlaylist(fs::path fullPath) {
	std::ifstream in(fullPath);

	std::string line;
	std::map<int, std::string> playlist;

	if (!in.is_open()) {
		playlist[0] = "OPEN_FAILED";
		std::cout << "Failed to open file\n";
		return playlist;
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

void static playPlaylist(fs::path fullPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	std::map<int, std::string> playlist = readPlaylist(fullPath);
	if (playlist[0] == "OPEN_FAILED") {
		return;
	}

	// loop through the whole playlist 
	for (auto& item : playlist) {
		std::string song = item.second;

		cmnd_load(song, decoder, device, deviceConfig, decoderInitialized);
		cmnd_play(decoder, device, deviceConfig, decoderInitialized);

		// "unrequests" skip after skipping
		skipRequested = false;

		// this some voodoo from chatgpt
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
	// cleans up playback when stop wasn't requested
	// (because when you skip the last song, it continues playing)
	if (!stopRequested) {
		cleanup(device, decoder, decoderInitialized);
	}

	// "unrequests" stop after playlist finishes
	stopRequested = false;
	std::cout << "DEBUG: exiting playlist thread\n";
}

void cmnd_playlist(std::string parameter, std::string& givenPath, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (parameter != "make" && parameter != "play") {
		std::cout << "Invalid command. Available 'playlist' options: 'make', 'play'\n";
		return;
	}

	givenPath.erase(remove(givenPath.begin(), givenPath.end(), '\"'), givenPath.end());

	fs::path path(givenPath);		  // convert string to path
	fs::path file("playlist.txt");	  // define file name
	fs::path fullPath = path / file;  // append file

	if (parameter == "make") {
		makePlaylistFile(path, fullPath);
	}
	else if (parameter == "play") {
		// start seperate thread so that the user can still run commands while the playlist is running
		std::thread(playPlaylist, path, std::ref(decoder), std::ref(device), std::ref(deviceConfig), std::ref(decoderInitialized)).detach();
	}
}

// function for skipping a playlist song
void cmnd_skip(ma_decoder& decoder) {
	skipRequested = true;
	playNextSound.notify_one();
}
