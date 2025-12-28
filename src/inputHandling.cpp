#include "../include/inputHandling.h"

#include "../include/helpCommand.h"     // cmnd_help
#include "../include/playbackControl.h" // cmnd_{load, play, stopPause}
#include "../include/seeking.h"         // cmnd_seek
#include "../include/volumeControl.h"   // cmnd_volume
#include "../include/time.h"            // cmnd_elapsedTime
#include "../include/playlist.h"        // cmnd_playlist

#include <iostream>
#include <string>
#include <sstream>

struct Command {
	std::string name;
	std::string parameter;
	std::string parameter2;
};

Command static getInput() {
	std::string userInput;
	Command cmnd;

	std::getline(std::cin, userInput);
	std::istringstream iss(userInput);

	std::getline(iss, cmnd.name, ' ');
	std::getline(iss, cmnd.parameter, ' ');
	std::getline(iss, cmnd.parameter2);

	return cmnd;
}

void static handleCommands(Command cmnd, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (cmnd.name == "help") cmnd_help();
	else if (cmnd.name == "load") cmnd_load(cmnd.parameter, decoder, device, deviceConfig, decoderInitialized);
	else if (cmnd.name == "play") cmnd_play(decoder, device, deviceConfig, decoderInitialized);
	else if (cmnd.name == "pause" || cmnd.name == "stop") cmnd_stopPause(cmnd.name, device, decoder, decoderInitialized);
	else if (cmnd.name == "seek") cmnd_seek(cmnd.parameter, decoder, device);
	else if (cmnd.name == "volume") cmnd_volume(cmnd.parameter);
	else if (cmnd.name == "elapsed") cmnd_elapsedTime(decoder);
	else if (cmnd.name == "playlist") cmnd_playlist(cmnd.parameter, cmnd.parameter2, decoder, device, deviceConfig, decoderInitialized);
	else if (cmnd.name == "skip") cmnd_skip(decoder);

	else std::cout << "Unknown command. Type 'help' for available commands\n";
}

void processUserInput(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	while (true)
	{
		Command cmnd = getInput();
		if (cmnd.name == "exit") break;
		handleCommands(cmnd, decoder, device, deviceConfig, decoderInitialized);
	}
}
