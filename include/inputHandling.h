#pragma once

#include "miniaudio.h"

struct Command;
Command static getInput();
void static handleCommands(Command cmnd, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
void processUserInput(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
