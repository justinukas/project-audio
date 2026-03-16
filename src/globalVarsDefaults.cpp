#include "../include/globalVars.hpp"
std::mutex audioMutex;
std::atomic<bool> soundIsPlaying = false;
std::atomic<bool> playbackFinished = false;
std::atomic<bool> stopRequested = false;
std::atomic<bool> skipRequested = false;
std::atomic<double> volumeMultiplier = 0.2;
