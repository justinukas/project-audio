#include "../include/audioSetup.hpp"
#include "../include/dataCallback.hpp"

bool AudioDevice::initialize(AudioDecoder decoder) {
	ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = decoder.getFormat();
    config.playback.channels = decoder.getChannels();
    config.sampleRate = decoder.getSampleRate();
    config.dataCallback = dataCallback.dataCallback;
    config.pUserData = master;

	if (ma_device_init(NULL, &config, &device) == MA_SUCCESS) {
		return true;
	}
	else { return false; }
}

