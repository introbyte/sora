// audio.h

#ifndef AUDIO_H
#define AUDIO_H

// includes
#include <mmdeviceapi.h>
#include <audioclient.h>

// defines

// typedefs

typedef void (*audio_callback_function)(f32*, i32);

// enums

enum audio_format {
	audio_format_f32
};

// structs

struct audio_params_t {
	u32 frequency = 44100;
	audio_format format = audio_format_f32;
	u32 channels = 2;
	u32 samples = 1024;
	audio_callback_function callback_function;
};

struct audio_state_t {
	IMMDeviceEnumerator* device_enumerator;
	IMMDevice* audio_device;
	IAudioClient3* audio_client;
	IAudioRenderClient* audio_render_client;

	u32 buffer_size;
	HANDLE buffer_ready;

	audio_params_t params;

	os_thread_t* audio_thread;
	b8 thread_running;
};

// global

global audio_state_t audio_state;

// functions

function void audio_init(audio_params_t);
function void audio_release();
function void audio_thread_function();

#endif // AUDIO_H