// main.cpp
#include "include/main.h"
#include "include/utilities.h"
#include "include/commands.h"

extern bool playing;

int main() {
    ma_result result;
    ma_decoder decoder;
    ma_device_config deviceConfig;
    ma_device device;

    std::string userInput, cmd, parameter;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    // while loop for user input
    while (true)
    {
        std::getline(std::cin, userInput);
        std::stringstream ss(userInput);
        std::getline(ss, cmd, ' ');
        std::getline(ss, parameter);

        if (cmd == "help")cmnd_help();
        else if (cmd == "load") cmnd_load(parameter, result, decoder, deviceConfig, device);
        else if (cmd == "play") cmnd_play(result, decoder, deviceConfig, device);
        else if (cmd == "stop" || cmd == "pause") cmnd_stoppause(decoder, device, cmd);
        else if (cmd == "seek") cmnd_seek(decoder, device, parameter);
        else if (cmd == "volume") cmnd_volume(decoder, device, parameter);
        else if (cmd == "exit") break;
        else if (cmd == "isplaying") std::cout << std::boolalpha << playing << "\n";
        else std::cout << "Unknown command. Type 'help' for available commands\n";
    }
    std::cout << "Exiting... ";

    if (playing) {
        ma_device_stop(&device);
        playing = false;
    }
    deviceCleanup(decoder, device);

    return 0;
}
