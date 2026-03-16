#pragma once

#include "miniaudio.h"

class AudioDevice {
private:
	ma_device device;

public:
  	ma_bool32 isStarted() const { return ma_device_is_started(&device); }
	bool start() {
		if (ma_device_start(&device) == MA_SUCCESS) {
			return true;
		}
		else { return false; }
	}
  	void stop() { ma_device_stop(&device); }

	
	bool initialize(const ma_device_config& config) {
		if (ma_device_init(NULL, &config, &device) == MA_SUCCESS) {
			return true;
		}
		else { return false; }
	}
  	void uninit() { ma_device_uninit(&device); }
};

class AudioDecoder {
private:
  	ma_decoder decoder;
  	ma_result result;

public:
  	void uninit() { ma_decoder_uninit(&decoder); }
	void seek(ma_uint64 frame) { ma_decoder_seek_to_pcm_frame(&decoder, frame); }
	ma_result initFile(const char* file) { return ma_decoder_init_file(file, NULL, &decoder); }

	// Results
	ma_result getResult() { return result; }
	void setResult(ma_result r) { result = r; }
	void clearResult() { result = MA_ERROR; }

	// For device configuration
	ma_format getFormat() { return decoder.outputFormat; }
	ma_uint32 getChannels() { return decoder.outputChannels; }
	ma_uint32 getSampleRate() { return decoder.outputSampleRate; }

	void getElapsedFrames(ma_uint64* frames) { ma_decoder_get_cursor_in_pcm_frames(&decoder, frames); }
	void getAudioLength(ma_uint64* frames) { ma_decoder_get_length_in_pcm_frames(&decoder, frames); }
};
