#pragma once

#include "../libraries/miniaudio.h"

#include "dataCallback.hpp"

class AudioDecoder {
private:
  	ma_decoder decoder;
  	ma_result result = MA_ERROR;

	bool initialized;

public:
	// Initialization
  	void uninit() {
		if (ma_decoder_uninit(&decoder) == MA_SUCCESS) {
			initialized = false;
		}
	}
	ma_result initFile(const char* file) {
		if (ma_decoder_init_file(file, NULL, &decoder) == MA_SUCCESS) {
			initialized = true;
			return MA_SUCCESS;
		}
		else return MA_ERROR;
	}

	// Results
	ma_result getResult() { return result; }
	void setResult(ma_result r) { result = r; }
	void clearResult() { result = MA_ERROR; }

	// For device configuration
	ma_format getFormat() { return decoder.outputFormat; }
	ma_uint32 getChannels() { return decoder.outputChannels; }
	ma_uint32 getSampleRate() { return decoder.outputSampleRate; }

	// Frame related
	ma_result seek(ma_uint64 frame) { return ma_decoder_seek_to_pcm_frame(&decoder, frame); }
	void getAudioLength(ma_uint64* frames) { ma_decoder_get_length_in_pcm_frames(&decoder, frames); }

	ma_decoder* decoderPointer() { return &decoder; }

	~AudioDecoder() {
		if (initialized) {
			uninit();
		}
	}
};

class AudioMaster;

class AudioDevice {
private:
	AudioMaster* master;

	ma_device device;
	DataCallback dataCallback;

	bool initialized;
public:
	AudioDevice(AudioMaster* m) : master(m){}


  	ma_bool32 isStarted() const { return ma_device_is_started(&device); }
	bool start() {
		if (ma_device_start(&device) == MA_SUCCESS) {
			return true;
		}
		else { return false; }
	}
  	void stop() { ma_device_stop(&device); }
	
	bool initialize(AudioDecoder& decoder) {
		ma_device_config config = ma_device_config_init(ma_device_type_playback);
    	config.playback.format = decoder.getFormat();
    	config.playback.channels = decoder.getChannels();
    	config.sampleRate = decoder.getSampleRate();
    	config.dataCallback = dataCallback.dataCallback;
    	config.pUserData = master;

		if (ma_device_init(NULL, &config, &device) == MA_SUCCESS) {
			initialized = true;
			return true;
		}
		else { return false; }
	}
  	void uninit() {
		initialized = false;
		ma_device_uninit(&device);
	}

	~AudioDevice() {
		if (initialized) {
			uninit();
		}
	}
};
