// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "miniaudio.h"

void cleanup(ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized);
int main();
