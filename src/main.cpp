#define MINIAUDIO_IMPLEMENTATION
#include "../libraries/miniaudio.h"
#include "../include/commandParsing.hpp"
#include "../include/commandExecutor.hpp"
//#include "../include/audioMaster.hpp"

int main() {
    CommandParser parser;
    Executor executor;
    //AudioMaster master;

    msg("Welcome to project-audio! Type 'help' for commands");
    while(true) {
        Command command = parser.parsedInput();
        std::string runResult = "break";//executor.run(master, command);

        if (runResult == "continue") {
            continue;
        }
        else if (runResult == "break") {
            break;
        }
    }
}
