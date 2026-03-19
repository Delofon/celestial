#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__unix__) || \
    ( defined(__APPLE__) && defined(__MACH__) ) || \
    defined(__linux__)
#define PLATFORM posix
#elif defined(_WIN32)
#define PLATFORM mswin
#endif

#ifndef PLATFORM
#error Could not detect PLATFORM! \
Platform does not define: \
__unix__, __APPLE__, __MACH__, __linux__ or _WIN32!
#endif

#include <stdint.h>
#include <unistd.h>

#if PLATFORM == posix
#define posix_us_sleep(x) usleep(x);
#elif PLATFORM == mswin
void mswin_us_sleep(uint64_t us);
#endif

static inline void us_sleep(uint64_t us)
{
#if PLATFORM == posix
    posix_us_sleep(us);
#elif PLATFORM == mswin
    mswin_us_sleep(us);
#endif
}

#endif

