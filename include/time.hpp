#pragma once

#include "miniaudio.h"

#include <string>

struct Time {
    int minutes;
    int seconds;
};

Time getTime(std::string type, ma_decoder& decoder);
void cmnd_elapsedTime(ma_decoder& decoder);
