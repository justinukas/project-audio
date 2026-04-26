#include "audioPlayer.hpp"
#include "commandParsing.hpp"
#include <thread>

class Executor {
private:
    void outputHelp() {
        std::ostringstream oss;
        oss
            << "Available commands:\n"
            << "BASIC:\n"
            << "help\n"
            << "load <file_directory>                     -load the audio file\n" /* (only .wav, .mp3, and .flac are supported) -idk for sure tho*/
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
public:
    std::string run(AudioPlayer& player, const Command& cmnd) {
        // do a while loop so that the user can always input a command
        if (cmnd.type == "help") outputHelp();
        else if (cmnd.type == "load") {
            if (playlistMode) {
                msg("Please exit playlist mode first.");
                return "continue";
            }
            player.initializeFile(cmnd.parameter1); 
        }
        else if (cmnd.type == "play" && !playlistMode) player.play();
        else if (cmnd.type == "pause") player.pause();
        else if (cmnd.type == "stop") player.stop();
        else if (cmnd.type == "seek") player.seek(cmnd.parameter1);
        else if (cmnd.type == "volume") player.setVolume(cmnd.parameter1);
        else if (cmnd.type == "elapsed") player.getElapsedTime();
        else if (cmnd.type == "length") player.getSongLength();

        else if (cmnd.type == "playlist") { playlistMode = true; std::cout << "PLAYLIST> "; }
        else if (cmnd.type == "make" && playlistMode) player.makePlaylist(cmnd.parameter1);
        else if (cmnd.type == "play" && playlistMode) std::thread(&AudioPlayer::playPlaylist, &player, cmnd.parameter1).detach();
        else if (cmnd.type == "skip") player.skipPlaylistSong();

        else if (cmnd.type == "exit" && playlistMode) playlistMode = false; 
        else if (cmnd.type == "exit" && !playlistMode) return "break";
        else msg("Unknown command. Type 'help' for available commands");
        return "";
    }
};
