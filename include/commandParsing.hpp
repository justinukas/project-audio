#pragma once

#include "audioPlayer.hpp"

#include <sstream>

extern bool playlistMode;

struct Command {
    std::string name;
    std::string parameter1;
    std::string parameter2;
};

class CommandParser {
private:
    // Helper methods
    static void stripQuotesApostrophes(std::string& path) {
	    if ((path.size() >= 2) && 
		   ((path.front() == '"' && path.back() == '"') || 
		    (path.front() == '\'' && path.back() == '\''))) {

		    path = path.substr(1, path.size() - 2);
        }
	}

    const Command parsedInput()  {
        std::string userInput;
	    Command command;

		if (playlistMode) std::cout << "PLAYLIST> ";
	    std::getline(std::cin, userInput);

    	std::istringstream iss(userInput);
    	std::getline(iss, command.name, ' ');
    	// for support of spaces in directories
    	command.name == "load"
    		? std::getline(iss, command.parameter1)
    		: std::getline(iss, command.parameter1, ' ');
	
    	std::getline(iss, command.parameter2);

    	// strip quotes and apostrophes in case the parameters are paths
    	stripQuotesApostrophes(command.parameter1);
    	stripQuotesApostrophes(command.parameter2);

	    return command;
    }

public:
    void run(AudioPlayer& player);
};
