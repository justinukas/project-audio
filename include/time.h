#pragma once

#include "miniaudio.h"

#include <string>

struct Time {
    int minutes;
    int seconds;
};

Time getTime(ma_decoder& decoder, std::string type);
void cmnd_elapsedTime(ma_decoder& decoder);
