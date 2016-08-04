#include "utils/time.h"
#include <time.h>

//namespace mbm {
//
//	/* Get current time in ns. */
//	uint64_t get_now( struct timeval *time, unsigned short debug ) {
//		struct timespec time;
//		clock_gettime(CLOCK_MONOTONIC, &time);
//		return static_cast<uint64_t>(time.tv_sec) * NS_PER_SEC + time.tv_nsec;
//	}
//
//}