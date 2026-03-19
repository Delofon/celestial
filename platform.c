#include <stdint.h>

#include "platform.h"

#if PLATFORM == posix
// skip
#elif PLATFORM == mswin
#error TODO: mswin sleep
void mswin_us_sleep(uint64_t us)
{
    // use waitable timers
}
#endif

