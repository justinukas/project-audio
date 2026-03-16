#include <mutex>
#include <atomic>
extern std::mutex audioMutex;  // use this when reading frames outside of data callback in any way (like seeking)
extern std::atomic<bool> soundIsPlaying;
extern std::atomic<bool> playbackFinished;
extern std::atomic<bool> stopRequested;
extern std::atomic<bool> skipRequested;
extern std::atomic<double> volumeMultiplier;
