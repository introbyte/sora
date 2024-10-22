// os.cpp

#ifndef OS_CPP
#define OS_CPP

// backend implementation

#if defined(OS_BACKEND_WIN32)
#include "backends/os/os_win32.cpp"
#elif defined(OS_BACKEND_LINUX)
#include "backends/os/os_linux.cpp"
#elif defined(OS_BACKEND_MACOS)
#include "backends/os/os_macos.cpp"
#else
#error undefined os backend.
#endif

#endif // OS_CPP