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
    std::vector<fs::path> shuffledQueue;
    std::atomic<bool> isShuffled;

    std::atomic<int> currentSong; // index of current song
    std::atomic<bool> skipCurrent;

    bool playQueueSong(const fs::path& song, AudioMaster& master, const SharedAudioState& sharedState);

public:
    void addToQueue(const fs::path& songToAdd);
    void removeFromQueue(const int& index);

    // call this in a thread
    void playQueue(AudioMaster& master, SharedAudioState& sharedState, AudioDecoder& decoder);

    // this feature might be finnicky with the for loop that will be used to play the queue
    // maybe use vectors with the insert function
    void moveSong(const int& oldPositionIndex, const int& newPositionIndex) {
        // copy song at old position
        // erase the song at old position
        // reinsert the song at new position
    }

    // toggle between shuffled and unshuffled
    // or add a flag that the queue is shuffled, 
    //               so that the copy of the original queue isn't overwritten by the shuffled one when shuffling again and again
    // how to shuffle:
    // auto rng = std::default_random_engine {};
    // std::ranges::shuffle(cards_, rng);
    void shuffleQueue();

    void playPreviousTrack();
    void playNextTrack(SharedAudioState& sharedState);

    // to play a specific song thats in the queue and continue the queue from there
    void selectQueueSong(const int& index);

    // output the queue on the screen
    void listQueue();

    // save queue as a file
    void saveAsFile();

    // read queue from file
    void readFromFile();
};
