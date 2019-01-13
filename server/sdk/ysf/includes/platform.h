#ifndef YSF_PLATFORM_H
#define YSF_PLATFORM_H

#include <stdlib.h>
#include <string.h>
#include "types.h"

#ifdef _WIN32
inline void LoadTickCount() {}
#else
#include "sys/time.h"
timeval startTime;
timeval currentTime;
DWORD GetTickCount() {
	gettimeofday(&currentTime, 0);
	return (currentTime.tv_usec - startTime.tv_usec) / 1000 + 1000 * (currentTime.tv_sec - startTime.tv_sec);
}
void LoadTickCount() {
	gettimeofday(&startTime, 0);
}
#endif

#endif