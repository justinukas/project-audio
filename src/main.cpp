#include "../include/main.h"
#include "../include/inputHandling.h"   // processUserInput
#include "../include/dataCallback.h"    // soundIsPlaying

#include <chrono>
#include <iostream>
#include <thread>

// cleanup devices after end of playback
/*void cleanup(ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized) {
    while (true) {
        //const auto pauseInterval = std::chrono::milliseconds(1);
        //std::this_thread::sleep_for(pauseInterval);

        // clean up devices if end of playback and the device is started
        if (!soundIsPlaying && ma_device_is_started(&device)) {
            ma_device_stop(&device);
            ma_device_uninit(&device);
            ma_decoder_uninit(&decoder);
            decoderInitialized = MA_ERROR;
            std::cout << "Cleaned up\n";
        }
    }
}*/

int main() {
    ma_decoder decoder;
    ma_device device;
    ma_device_config deviceConfig;
    ma_result decoderInitialized = MA_ERROR;

    std::cout << "Welcome to the gaming time audio player TM! Type 'help' for a list of commands\n";
    
    //std::thread(cleanup, std::ref(device), std::ref(decoder), std::ref(decoderInitialized)).detach();
    processUserInput(decoder, device, deviceConfig, decoderInitialized);
    
    std::cout << "Exiting... ";
}
