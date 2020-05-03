/**
 * @file rtcc.c
 * Realtime clock handling functions
 */

#include <sys/time.h>

int gettimeofday(struct timeval *tv, void *tz __attribute__((unused))) {
    return 0;
}
