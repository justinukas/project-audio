#include "../include/globalVars.hpp"
std::mutex audioMutex;
std::atomic<bool> soundIsPlaying = false;
std::atomic<bool> playbackFinished = false;
std::atomic<double> volumeMultiplier = 0.2;
std::atomic<bool> playlistMode = false;
