#ifndef TS_H_
#define TS_H_

#include <time.h>

#define NS_PER_S 1000000000
#define NS_PER_S_DBL 1000000000.0

#define CLOCK(ts) clock_gettime(CLOCK_MONOTONIC, &ts)

double tstosdbl(const struct timespec *ts);

// a - b
int tsdiff(
        struct timespec *restrict ts,
        const struct timespec *restrict a,
        const struct timespec *restrict b
        );

#endif

