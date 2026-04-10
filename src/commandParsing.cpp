#include "../include/commandParsing.hpp"
#include "../include/outputProcessor.hpp"
#include <thread>

void outputHelp() {
    std::ostringstream oss;
    oss
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
    msg(oss.str());
}

void CommandParser::run(AudioPlayer& player) {    
    //std::thread t(processOutput, &player);
    //t.detach();

    // do a while loop so that the user can always input a command
	while (true) {
		Command cmnd = parsedInput();

        if (cmnd.name == "help") outputHelp();
        else if (cmnd.name == "load") {
            if (playlistMode) {
                msg("Please exit playlist mode first.");
                continue;
            }
            player.initializeFile(cmnd.parameter1); 
        }
        else if (cmnd.name == "play" && !playlistMode) player.play();
        else if (cmnd.name == "pause") player.pause();
        else if (cmnd.name == "stop") player.stop();
        else if (cmnd.name == "seek") player.seek(cmnd.parameter1);
        else if (cmnd.name == "volume") player.setVolume(cmnd.parameter1);
        else if (cmnd.name == "elapsed") player.getElapsedTime();

        else if (cmnd.name == "playlist") { playlistMode = true; std::cout << "PLAYLIST> "; }//player.enablePlaylistMode();
        else if (cmnd.name == "make" && playlistMode) player.makePlaylist(cmnd.parameter1);
        else if (cmnd.name == "play" && playlistMode) std::thread(&AudioPlayer::playPlaylist, &player, cmnd.parameter1).detach();
        else if (cmnd.name == "skip") player.skipPlaylistSong();

        else if (cmnd.name == "exit" && playlistMode) playlistMode = false; 
        else if (cmnd.name == "exit" && !playlistMode) break;
        else msg("Unknown command. Type 'help' for available commands");
	}
}
