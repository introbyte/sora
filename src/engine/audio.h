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
	u32 samples = 256;
	audio_callback_function callback_function;
};

struct audio_state_t; // implement in backends

// functions

function void audio_init(audio_params_t);
function void audio_release();
function void audio_thread_function();

#define AUDIO_BACKEND_WASAPI

// per backend includes
#ifdef AUDIO_BACKEND_WASAPI
#include "backends/audio/audio_wasapi.h"
#elif defined(AUDIO_BACKEND_COREAUDIO)
// not implemented
#elif defined(AUDIO_BACKEND_ALSA)
// not implemented
#endif 

#endif // AUDIO_H