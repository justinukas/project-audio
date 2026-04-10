#include "../include/globalVars.hpp"
#include <mutex>
#include <string>
#include <iostream>

std::mutex queueMutex;

void msg(const std::string& message) {
    std::lock_guard<std::mutex> lock(queueMutex);

    // move to start of line and clear any existing user typing
    std::cout << "\r\x1B[K";

    // print message
    std::cout << message << '\n';
    if (playlistMode)  {
        std::cout << "PLAYLIST> " << std::flush;
    }
}
