// audio.h

#ifndef AUDIO_H
#define AUDIO_H

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

// state (implementate per backend)
function void audio_init(audio_params_t);
function void audio_release();
function void audio_thread_function();

// per backend includes
#ifdef AUD_BACKEND_WASAPI
#include "backends/audio/audio_wasapi.h"
#elif defined(AUD_BACKEND_CORE_AUDIO)
// not implemented
#elif defined(AUD_BACKEND_ALSA)
// not implemented
#endif 

#endif // AUDIO_H