#include "../include/main.h"
#include "../include/inputHandling.h"
//#include "../include/dataCallback.h"

//#include <chrono>
#include <iostream>
//#include <thread>

int main() {
	ma_decoder decoder;
	ma_device device;
	ma_device_config deviceConfig;
	ma_result decoderInitialized = MA_ERROR;

	std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";

	processUserInput(decoder, device, deviceConfig, decoderInitialized);

	std::cout << "Exiting... ";
}
