#include <mutex>
#include <string>
#include <iostream>

std::mutex queueMutex;

void msg(const std::string& message, const bool& playlistMode) {
    std::lock_guard<std::mutex> lock(queueMutex);

    // move to start of line and clear any existing user typing
    std::cout << "\r\x1B[K";

    // print message
    std::cout << message << '\n';
    if (playlistMode)  {
        std::cout << "PLAYLIST> " << std::flush;
    }
}
