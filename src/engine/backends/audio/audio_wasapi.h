// audio_wasapi.h

#ifndef AUDIO_WASAPI_H
#define AUDIO_WASAPI_H

// includes
#include <mmdeviceapi.h>
#include <audioclient.h>

// structs

struct audio_state_t {
	IMMDeviceEnumerator* device_enumerator;
	IMMDevice* audio_device;
	IAudioClient3* audio_client;
	IAudioRenderClient* audio_render_client;

	u32 buffer_size;
	HANDLE buffer_ready;

	audio_params_t params;

	os_handle_t audio_thread;
	b8 thread_running;
};

// globals
global audio_state_t audio_state;

// functions



#endif // AUDIO_WASAPI_H