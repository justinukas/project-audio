#include "../include/playlist.h"
#include "../include/playbackControl.h" // cmnd_{load, play}(), stopRequested
#include "../include/dataCallback.h"    // soundIsPlaying
#include "../include/seeking.h"         // seekToFrame
#include "../include/time.h"            // Time, getLength()

#include <vector>
#include <fstream>
#include <thread>
#include <filesystem>
#include <iostream>

bool skipRequested = false;

void static makePlaylistFile(std::string directory) {
    if (!std::filesystem::exists(directory)) {
        std::cout << "Invalid path\n";
        return;
    }

    std::vector<std::string> songPaths;

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        auto extension = entry.path().extension();
        if (extension == ".mp3" || extension == ".wav" || extension == ".flac") {
            std::cout << entry.path() << std::endl;
            songPaths.push_back(entry.path().string());
        }
    }
    std::ofstream out(directory + "/playlist.txt");
    for (int i = 0; i < songPaths.size(); i++) {
        out << i + 1 << ' ' << songPaths[i] << std::endl;
    }
    out.close();
}

std::vector<std::string> readPlaylist(std::string directory) {
    std::ifstream in(directory + "/playlist.txt");
    std::string line;
    std::vector<std::string> playlist;

    while (std::getline(in, line))
    {
        std::string number;
        std::string path;

        std::stringstream ss(line);
        std::getline(ss, number, ' ');
        std::getline(ss, path);

        playlist.push_back(path);
    }
    return playlist;
}

void static playPlaylist(std::string directory, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
    std::vector<std::string> playlist = readPlaylist(directory);

    std::ifstream in(directory + "\\playlist.txt");
    // add support for forward slash

    if (!in.is_open()) {
        std::cerr << "Failed to open file\n";
        return;
    }

    for (std::string &song : playlist) {
        skipRequested = false;
        //std::cout << song << std::endl;
        cmnd_load(song, decoder, device, deviceConfig, decoderInitialized);
        cmnd_play(decoder, device, deviceConfig, decoderInitialized);

        // this some voodoo from chatgpt for making the threat sleep until sound is no longer playing
        std::unique_lock<std::mutex> lock(audioMutex);
        playNextSound.wait(lock, [] {
            return !soundIsPlaying || skipRequested;
        });

        if (stopRequested) { break; }
    }
    if (!stopRequested) { 
        cleanup(device, decoder, decoderInitialized); 
    }
    stopRequested = false;
    std::cout << "DEBUG: exiting playlist thread\n";
}

void cmnd_playlist(std::string parameter, std::string& directory, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) { 
    if (parameter != "make" && parameter != "play") {
        std::cout << "Invalid command. Available 'playlist' options: 'make', 'play'\n";
        return;
    }

    directory.erase(remove(directory.begin(), directory.end(), '\"'), directory.end());
    if (parameter == "make") {
        makePlaylistFile(directory);
    }
    else if (parameter == "play") {
        // start seperate thread so that the user can still run commands while the playlist is running
        std::thread(playPlaylist, directory, std::ref(decoder), std::ref(device), std::ref(deviceConfig), std::ref(decoderInitialized)).detach();
    }
}

void cmnd_skip(ma_decoder& decoder) {
    skipRequested = true;
    playNextSound.notify_one();
}
