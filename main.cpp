// main.cpp
#include "include/main.h"
#include "include/utilities.h"
#include "include/commands.h"

extern bool isPlaying;

int main() {
    ma_result result = {};
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    bool decoderInitialized = false;

    std::string userInput, cmd, parameter;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    // while loop for user input
    while (true)
    {
        std::getline(std::cin, userInput);
        std::stringstream ss(userInput);
        std::getline(ss, cmd, ' ');
        std::getline(ss, parameter);

        if (cmd == "help") cmnd_help();
        else if (cmd == "load") cmnd_load(parameter, result, decoder, deviceConfig, device, decoderInitialized);
        else if (cmd == "play") cmnd_play(result, decoder, deviceConfig, device, decoderInitialized);
        else if (cmd == "stop" || cmd == "pause") cmnd_stoppause(decoder, device, cmd);
        else if (cmd == "seek") cmnd_seek(decoder, device, parameter);
        else if (cmd == "volume") cmnd_volume(decoder, device, parameter);
        else if (cmd == "time") cmnd_elapsedTime(decoder);
        else if (cmd == "read") cmnd_readDirectory(parameter);
        else if (cmd == "exit") break;
        else if (cmd == "isplaying") std::cout << std::boolalpha << isPlaying << "\n";
        else std::cout << "Unknown command. Type 'help' for available commands\n";
    }
    std::cout << "Exiting... ";

    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
    }
    deviceCleanup(decoder, device, decoderInitialized);

    return 0;
}
