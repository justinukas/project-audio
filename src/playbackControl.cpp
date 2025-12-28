#include "../include/playbackControl.h"
#include "../include/dataCallback.h"
#include "../include/time.h"
#include "../include/seeking.h"

#include <iostream>
#include <iomanip>

bool stopRequested = false;

void cleanup(ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized) {
	std::lock_guard<std::mutex> lock(audioMutex);
	if (ma_device_is_started(&device)) {
		ma_device_stop(&device);
	}
	ma_device_uninit(&device);
	ma_decoder_uninit(&decoder);
	decoderInitialized = MA_ERROR;

	soundIsPlaying = false;
	playNextSound.notify_one();
	std::cout << "Cleaned up\n";
}

void cmnd_load(std::string& rawUserInput, ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (ma_device_is_started(&device)) {
		ma_device_uninit(&device);
		soundIsPlaying = false;
	}

	if (rawUserInput.empty()) {
		std::cout << "No file path provided\n";
		return;
	}

	if (decoderInitialized != MA_ERROR) {
		cleanup(device, decoder, decoderInitialized);
	}

	rawUserInput.erase(remove(rawUserInput.begin(), rawUserInput.end(), '\"'), rawUserInput.end());
	const char* audioPath = rawUserInput.c_str();

	decoderInitialized = ma_decoder_init_file(audioPath, NULL, &decoder);

	if (decoderInitialized != MA_SUCCESS) {
		std::cout << "Could not load file: \"" << audioPath << "\"\n";
		return;
	}

	std::cout << "File " << audioPath << " loaded successfuly!\n";
}

void static configureDevice(ma_decoder& decoder, ma_device_config& deviceConfig) {
	deviceConfig = ma_device_config_init(ma_device_type_playback);
	deviceConfig.playback.format = decoder.outputFormat;
	deviceConfig.playback.channels = decoder.outputChannels;
	deviceConfig.sampleRate = decoder.outputSampleRate;
	deviceConfig.dataCallback = data_callback;
	deviceConfig.pUserData = &decoder;
}

void static initializeDevice(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig) {
	if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
		std::cout << "Failed to open playback device\n";
		ma_decoder_uninit(&decoder);
		return;
	}

	if (ma_device_start(&device) != MA_SUCCESS) {
		std::cout << "Failed to start playback device\n";
		ma_device_uninit(&device);
		ma_decoder_uninit(&decoder);
		return;
	}
}

void cmnd_play(ma_decoder& decoder, ma_device& device, ma_device_config& deviceConfig, ma_result& decoderInitialized) {
	if (decoderInitialized != MA_SUCCESS) {
		std::cout << "Please load a file using 'load <file_path>' first!\n";
		return;
	}

	// start from the beginning if something is already playing
	if (soundIsPlaying) {
		const ma_uint64 beginningFrame = 0;
		seekFrame(decoder, beginningFrame);
		return;
	}

	configureDevice(decoder, deviceConfig);
	initializeDevice(decoder, device, deviceConfig);

	soundIsPlaying = true;

	Time length = getTime("length", decoder);
	std::cout << "Playing..." << '\n'
		<< "Song length: "
		<< '[' << std::setfill('0') << std::setw(2) << length.minutes << ':' << std::setfill('0') << std::setw(2) << length.seconds << ']' << '\n';
}

void cmnd_stopPause(std::string input, ma_device& device, ma_decoder& decoder, ma_result& decoderInitialized) {
	if (soundIsPlaying) {
		ma_device_stop(&device);

		if (input == "pause") {
			std::cout << "Paused\n";
		}
		else if (input == "stop") {
			if (decoderInitialized != MA_ERROR) {
				cleanup(device, decoder, decoderInitialized);
			}

			stopRequested = true;
			std::cout << "Stopped playback\n";
		}
	}
}
