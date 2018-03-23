#include "win_t.h"

static u_int64_t
filetime_to_unix_epoch(const FILETIME *ft) {
	u_int64_t res = (u_int64_t)ft->dwHighDateTime << 32;
	res |= ft->dwLowDateTime;
	res /= 10;                   /* from 100 nano-sec periods to usec */
	res -= DELTA_EPOCH_IN_USEC;  /* from Win epoch to Unix epoch */
	return (res);
}

int
gettimeofday(struct timeval *tv, void *tz) {
	FILETIME  ft;
	u_int64_t tim;
	if (!tv) {
		//errno = EINVAL;
		return (-1);
	}
	GetSystemTimeAsFileTime(&ft);
	tim = filetime_to_unix_epoch(&ft);
	tv->tv_sec = (long)(tim / 1000000L);
	tv->tv_usec = (long)(tim % 1000000L);
	return (0);
}

long long
tvdiff_us(struct timeval *tv1, struct timeval *tv2) {
	struct timeval delta;
	delta.tv_sec = tv1->tv_sec - tv2->tv_sec;
	delta.tv_usec = tv1->tv_usec - tv2->tv_usec;
	if (delta.tv_usec < 0) {
		delta.tv_sec--;
		delta.tv_usec += 1000000;
	}
	return 1000000LL * delta.tv_sec + delta.tv_usec;
}