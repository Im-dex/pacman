#include "timer.h"
#include "error.h"

#include <time.h>

namespace Pacman {

uint64_t Timer::GetCurrentTime()
{
	timespec now;
	int res = clock_gettime(CLOCK_MONOTONIC, &now);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::TimerFailed);
	return now.tv_sec*1000000000LL + now.tv_nsec;
}

} // Pacman namespace
