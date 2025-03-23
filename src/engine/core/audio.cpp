// audio.cpp

#ifndef AUDIO_CPP
#define AUDIO_CPP

//-  per backend includes

#ifdef AUD_BACKEND_WASAPI
#    include "backends/audio/audio_wasapi.cpp"
#elif defined(AUD_BACKEND_CORE_AUDIO)
// not implemented
#elif defined(AUD_BACKEND_ALSA)
// not implemented
#endif 

#endif // AUDIO_CPP