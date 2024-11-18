// audio.cpp

#ifndef AUDIO_CPP
#define AUDIO_CPP

// per backend includes
#ifdef AUDIO_BACKEND_WASAPI
#include "backends/audio/audio_wasapi.cpp"
#elif defined(AUDIO_BACKEND_COREAUDIO)
// not implemented
#elif defined(AUDIO_BACKEND_ALSA)
// not implemented
#endif 


#endif // AUDIO_CPP