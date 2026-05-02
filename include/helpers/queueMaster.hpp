#pragma once

#include <filesystem>
#include <vector>
#include <atomic>

namespace fs = std::filesystem;

class QueueMaster {
private:
    // index is position in the queue
    // element is the path to the song
    std::vector<fs::path> songQueue;
    std::vector<fs::path> shuffledQueue;
    std::atomic<bool> isShuffled;

public:
    void addToQueue(fs::path songToAdd);
    void removeFromQueue();

    // execute this in a thread
    // will be structurally similar to playlist playback function
    void playQueue();


    // this feature might be finnicky with the for loop that will be used to play the queue
    // maybe use vectors with the insert function
    void moveSong(int oldPositionIndex, int newPositionIndex) {
        // copy song at old position
        // erase the song at old position
        // reinsert the song at new position
    }

    // toggle between shuffled and unshuffled
    // or add a flag that the queue is shuffled, 
    //               so that the copy of the original queue isn't overwritten by the shuffled one when shuffling again and again
    void shuffleQueue();

    void playPreviousTrack();
    // basically a skip function
    void playNextTrack();
    // to play a specific song thats in the queue and continue the queue from there
    void selectQueueSong(int index);
};
