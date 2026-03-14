#pragma once

#include "miniaudio.h"

struct Command;
Command getInput();
void processUserInput(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized);
