#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"
#include "../include/commandParsing.hpp"
#include "../include/commandExecutor.hpp"
#include "../include/audioPlayer.hpp"

int main() {
    CommandParser parser;
    Executor executor;
    AudioPlayer player;

    msg("Welcome to project-audio! Type 'help' for commands");
    while(true) {
        Command command = parser.parsedInput();
        std::string runResult = executor.run(player, command);

        if (runResult == "continue") {
            continue;
        }
        else if (runResult == "break") {
            break;
        }
    }
}
