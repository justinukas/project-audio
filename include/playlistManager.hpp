#include "globalVars.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

class PlaylistManager {
public:
    // Reads songs from file
    std::map<int, std::string> playlist(fs::path filePath) {
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
};
