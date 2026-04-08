#include "../include/commandParsing.hpp"
#include <thread>
#include "../include/outputProcessor.hpp"

void outputHelp() {
    std::cout
        << "Available commands:\n"
        << "BASIC:\n"
        << "help\n"
        << "load <file_directory>                     -load the audio file\n" /*+ (only .wav, .mp3, and .flac are supported) -idk for sure tho*/
        << "                                          (TIP: you can drag and drop your file into the command line to quickly input the directory!)\n"
        << "play                                      -start or resume playback\n"
        << "stop                                      -stop playback (stops playlist too)\n"
        << "pause                                     -pause playback\n"
        << "seek                                      -seek to a specific timestamp. Syntax: mm:ss\n"
        << "volume                                    -set volume from 0-100\n"
        << "elapsed                                   -get how much of the song has elapsed\n"
        << "exit                                      -exit program\n"

        << "PLAYLIST:\n"
        << "playlist                                  -enable playlist mode\n"
        << "make <folder_directory>                   -read the directories of music files in specified directory and make a playlist file based on said diretories\n"
        << "play <playlist_file_directory>            -play a playlist of songs based on the file made with 'make' command\n"
        << "skip                                      -skip current playlist song\n"
        << "exit                                      -exit playlist mode\n"
        ;
}

void CommandParser::run(AudioPlayer& player) {    
    // do a while loop so that the user can always input a command
	while (true) {
        // process outputs BEFORE input begins
        processOutput();
        if (player.isPlaylistMode()) std::cout << "PLAYLIST> ";

		Command cmnd = parsedInput();

        if (cmnd.name == "help") outputHelp();
        else if (cmnd.name == "load") {
            if (player.isPlaylistMode()) {
                std::cout << "Please exit playlist mode first.";
                continue;
            }
            std::cout << cmnd.parameter1;
            player.initializeFile(cmnd.parameter1); 
        }
        else if (cmnd.name == "play" && !player.isPlaylistMode()) player.play();
        else if (cmnd.name == "pause") player.pause();
        else if (cmnd.name == "stop") player.stop();
        else if (cmnd.name == "seek") player.seek(cmnd.parameter1);
        else if (cmnd.name == "volume") player.setVolume(cmnd.parameter1);
        else if (cmnd.name == "elapsed") player.getElapsedTime();

        else if (cmnd.name == "playlist") player.enablePlaylistMode();
        else if (cmnd.name == "make" && player.isPlaylistMode()) player.makePlaylist(cmnd.parameter1);
        else if (cmnd.name == "play" && player.isPlaylistMode()) std::thread(&AudioPlayer::playPlaylist, &player, cmnd.parameter1).detach();
        else if (cmnd.name == "skip") player.skipPlaylistSong();

        else if (cmnd.name == "exit" && player.isPlaylistMode()) player.disablePlaylistMode(); 
        else if (cmnd.name == "exit" && !player.isPlaylistMode()) break;
        else queueMsg("Unknown command. Type 'help' for available commands\n");
	}
}
