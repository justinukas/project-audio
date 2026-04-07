#include "../include/commandParsing.hpp"

#include <queue>
#include <string>
#include <iostream>
#include <condition_variable>

std::queue<std::string> messageQueue;
std::mutex queueMutex;
std::condition_variable hasQueued;

void queueMsg(const std::string& msg) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(msg);
    hasQueued.notify_one();
}

void processOutput() {
    while (!messageQueue.empty()) {
        std::cout << messageQueue.front();
        messageQueue.pop();
    }
}
