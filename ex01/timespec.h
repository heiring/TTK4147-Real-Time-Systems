#ifndef TIMESPEC_H
#define TIMESPEC_H

#define _POSIX_C_SOURCE 199309L
#include <time.h>

extern struct timespec timespec_normalized(time_t sec, long nsec);
extern struct timespec timespec_sub(struct timespec lhs, struct timespec rhs);
extern struct timespec timespec_add(struct timespec lhs, struct timespec rhs);
extern int timespec_cmp(struct timespec lhs, struct timespec rhs);

#endif // TIMESPEC_H