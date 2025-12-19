#include <iostream>

void cmnd_help() {
    std::cout
        << "Available commands:\n"
        << "help\n"
        << "load <file_directory>  -loads the audio file\n" /*+ (only .wav, .mp3, and .flac are supported) -idk for sure tho*/
        << "                       (TIP: you can drag and drop your file into the command line!)\n"
        << "play                   -start or resume playback\n"
        << "stop                   -stop playback\n"
        << "pause                  -pause playback\n"
        << "seek                   -seek to a specific timestamp. Syntax: mm:ss\n"
        << "volume                 -set volume from 0-100\n"
        << "elapsed                -get how much of the song has elapsed\n"
        //<< "read                   -read a directory for multiple audio files and play them sequentially\n"
        << "exit                   -exit program\n"
        //<< "\nDEBUG COMMANDS:\n"

        //<< "isplaying              -check if audio is playing\n"

        ;
}
