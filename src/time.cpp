#include "../include/time.h"
#include "../include/dataCallback.h"

#include <iostream>
#include <iomanip>

Time getTime(std::string type, ma_decoder& decoder) {
	std::lock_guard<std::mutex> lock(audioMutex);
	int minutes = 0, seconds = 0;

	ma_uint64 frames = 0;
	if (type == "elapsed") {
		ma_decoder_get_cursor_in_pcm_frames(&decoder, &frames);
	}
	else if (type == "length") {
		ma_decoder_get_length_in_pcm_frames(&decoder, &frames);
	}

	// convert frames to seconds
	if (decoder.outputSampleRate != 0) {
		seconds = static_cast<int>(frames) / decoder.outputSampleRate;
	}

	minutes = seconds / 60;
	seconds -= minutes * 60;
	return { minutes, seconds };
}

void cmnd_elapsedTime(ma_decoder& decoder) {
	if (!soundIsPlaying) {
		return;
	}

	Time elapsed = getTime("elapsed", decoder);
	std::cout << '[' << std::setfill('0') << std::setw(2) << elapsed.minutes << ':' << std::setfill('0') << std::setw(2) << elapsed.seconds << ']' << '\n';
}
