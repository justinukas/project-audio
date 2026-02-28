#include "../include/inputHandling.h"

#include "../include/helpCommand.h"
#include "../include/playbackControl.h"
#include "../include/seeking.h"
#include "../include/volumeControl.h"
#include "../include/time.h"
#include "../include/playlist.h"

#include <iostream>
#include <string>
#include <sstream>

struct Command {
	std::string name;
	std::string parameter;
	std::string parameter2;
};

void stripQuotesApostrophes(std::string& path) {
	if ((path.size() >= 2) && 
		((path.front() == '"' && path.back() == '"') || 
		(path.front() == '\'' && path.back() == '\''))) {

		path = path.substr(1, path.size() - 2);
	}
}

Command static getInput() {
	std::string userInput;
	Command cmnd;

	std::getline(std::cin, userInput);

	std::istringstream iss(userInput);
	std::getline(iss, cmnd.name, ' ');
	// for support of spaces in directories
	cmnd.name == "load"
		? std::getline(iss, cmnd.parameter)
		: std::getline(iss, cmnd.parameter, ' ');
	
	std::getline(iss, cmnd.parameter2);

	// strip quotes and apostrophes in case the parameters are paths
	stripQuotesApostrophes(cmnd.parameter);
	stripQuotesApostrophes(cmnd.parameter2);

	return cmnd;
}

void processUserInput(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	// do a while loop so that the user can always input a command
	while (true) {
		Command cmnd = getInput();

		if (cmnd.name == "help") cmnd_help();
		else if (cmnd.name == "load") cmnd_load(cmnd.parameter, decoder, device, deviceConfig, decoderInitialized);
		else if (cmnd.name == "play") cmnd_play(decoder, device, deviceConfig, decoderInitialized);
		else if (cmnd.name == "pause" || cmnd.name == "stop") cmnd_stopPause(cmnd.name, device, decoder, decoderInitialized);
		else if (cmnd.name == "seek") cmnd_seek(cmnd.parameter, decoder, device);
		else if (cmnd.name == "volume") cmnd_volume(cmnd.parameter);
		else if (cmnd.name == "elapsed") cmnd_elapsedTime(decoder);
		else if (cmnd.name == "playlist") cmnd_playlist(cmnd.parameter, cmnd.parameter2, decoder, device, deviceConfig, decoderInitialized);
		else if (cmnd.name == "skip") cmnd_skip(decoder);

		else if (cmnd.name == "exit") break;
		else std::cout << "Unknown command. Type 'help' for available commands\n";
	}
}
