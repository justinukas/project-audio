#pragma once
#include "outputProcessor.hpp"

#include <sstream>
#include <iostream>

// if by the end its only 1 param left, remove param2
struct Command {
    std::string type;
    std::string parameter1;
    std::string parameter2;
};

class CommandParser {
private:
    // Helper methods
    void stripQuotesApostrophes(std::string& path) {
	    if ((path.size() >= 2) && 
		   ((path.front() == '"' && path.back() == '"') || 
		    (path.front() == '\'' && path.back() == '\''))) {

		    path = path.substr(1, path.size() - 2);
        }
	}
public:
    Command parsedInput()  {
        std::string userInput;
	    Command command;

	    std::getline(std::cin, userInput);

    	std::istringstream iss(userInput);
    	std::getline(iss, command.type, ' ');
    	// for support of spaces in directories
		if (command.type == "load" || command.type == "play") {
			std::getline(iss, command.parameter1);
		}
		else { std::getline(iss, command.parameter1, ' '); }
	
    	std::getline(iss, command.parameter2);

    	// strip quotes and apostrophes in case the parameters are paths
    	stripQuotesApostrophes(command.parameter1);
    	stripQuotesApostrophes(command.parameter2);

	    return command;
    }
};
