#pragma once

#include <filesystem>
#include <vector>
#include <atomic>

#include "audioPlayer.hpp"

class AudioMaster;

namespace fs = std::filesystem;

class QueueMaster {
private:
    // index is position in the queue
    // element is the path to the song
    std::vector<fs::path> songQueue;

    
    std::vector<fs::path> preShuffleQueue; // backup after shuffling
    std::atomic<bool> isShuffled = false;

    std::atomic<int> currentPosition; // index of current song
    std::atomic<bool> skipCurrent;

    bool playQueueSong(const fs::path& song, AudioMaster& master, const SharedAudioState& sharedState);

public:
    // queue editing/printing functions
    void addToQueue(const fs::path& songToAdd);
    void removeFromQueue(const int& position);
    void shuffleQueue();
    void moveSong(const int& oldPosition, const int& newPosition);
    void listQueue();
    void saveAsFile(const fs::path& destination); // maybe make file name choosing possible so that multiple playlists can exist in one folder
    void readFromFile(const fs::path& path);

    // queue playback functions
    void playQueue(AudioMaster& master, SharedAudioState& sharedState, AudioDecoder& decoder); // call this in a thread
    void playNextTrack(SharedAudioState& sharedState);
    void playPreviousTrack();
    void selectQueueSong(const int& position);
};
