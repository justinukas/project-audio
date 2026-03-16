#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"
#include "../include/commandParsing.hpp"
#include "../include/audioPlayer.hpp"

int main() {
    CommandParser parser;
    AudioPlayer player;

    parser.run(player);
}
