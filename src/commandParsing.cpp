#include "../include/commandParsing.hpp"

void outputHelp() {
    std::cout
        << "Available commands:\n"
        << "help\n"
        << "load <file_directory>                     -loads the audio file\n" /*+ (only .wav, .mp3, and .flac are supported) -idk for sure tho*/
        << "                                          (TIP: you can drag and drop your file into the command line to quickly input the directory!)\n"
        << "play                                      -start or resume playback\n"
        << "stop                                      -stop playback\n"
        << "pause                                     -pause playback\n"
        << "seek                                      -seek to a specific timestamp. Syntax: mm:ss\n"
        << "volume                                    -set volume from 0-100\n"
        << "elapsed                                   -get how much of the song has elapsed\n"
        << "playlist <make/play> <folder_directory>   -make/play a playlist of songs\n"
        << "skip                                      -skip current playlist song\n"
        // add playlist explanation

        << "exit                   -exit program\n"
        ;
}

void CommandParser::run(AudioPlayer player) {
	// do a while loop so that the user can always input a command
	while (true) {
		Command cmnd = parsedInput();

        if (cmnd.name == "help") outputHelp();
        else if (cmnd.name == "load") player.initializeFile(cmnd.parameter1);
        else if (cmnd.name == "play") player.play();
        else if (cmnd.name == "pause") player.pause();
        else if (cmnd.name == "stop") player.stop();
        else if (cmnd.name == "seek") player.seek(cmnd.parameter1);
        else if (cmnd.name == "volume") player.setVolume(cmnd.parameter1);
        else if (cmnd.name == "elapsed") player.getElapsedTime();
        else if (cmnd.name == "playlist") player.playlistMakerPlayer(cmnd.parameter1, cmnd.parameter2);
        else if (cmnd.name == "skip") player.skipPlaylistSong();

        else if (cmnd.name == "exit") break;
        else std::cout << "Unknown command. Type 'help' for available commands\n";
	}
}
