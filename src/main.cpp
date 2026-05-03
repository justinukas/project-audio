#define MINIAUDIO_IMPLEMENTATION
#include "../libraries/miniaudio.h"
#include "../include/commandParsing.hpp"
#include "../include/commandExecutor.hpp"
#include "../include/audioMaster.hpp"
#include "../include/outputProcessor.hpp"

int main() {
    //std::cout << "i work";
    CommandParser parser;
    Executor executor;
    AudioMaster master;

    //std::cout << "i work";
    msg("Welcome to project-audio! Type 'help' for commands");
    while(true) {
        Command command = parser.parsedInput();
        std::string runResult = executor.run(master, *master.statePointer(), command);

        if (runResult == "continue") {
            continue;
        }
        else if (runResult == "break") {
            break;
        }
    }
}
