#include <string.h>
#include <time.h>

#include "ts.h"

double tstosdbl(const struct timespec *ts)
{
    double s = ts->tv_sec;
    s += ts->tv_nsec / NS_PER_S_DBL;
    return s;
}

int tsdiff(
              struct timespec *restrict ts,
        const struct timespec *restrict a,
        const struct timespec *restrict b
        )
{
    memcpy(ts, a, sizeof(*ts));

    ts->tv_sec -= b->tv_sec;

    if(b->tv_nsec > ts->tv_nsec)
    {
        ts->tv_sec--;
        ts->tv_nsec += NS_PER_S;
    }

    ts->tv_nsec -= b->tv_nsec;

    return 1;
}

