#include "../include/commandParsing.hpp"

#include <queue>
#include <string>
#include <iostream>

std::queue<std::string> messageQueue;
std::mutex queueMutex;

void queueMsg(const std::string& msg) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(msg);
}

void processOutput() {
    while (!messageQueue.empty()) {
        std::cout << messageQueue.front();
        messageQueue.pop();
    }
}
