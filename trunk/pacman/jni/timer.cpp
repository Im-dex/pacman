#include "timer.h"

#include <ctime>

#include "error.h"

namespace Pacman {

uint64_t Timer::GetCurrentTime() const
{
	timespec now;
	const int res = clock_gettime(CLOCK_MONOTONIC, &now);
	PACMAN_CHECK_ERROR(res == 0);
	return now.tv_sec*1000000000LL + now.tv_nsec;
}

} // Pacman namespace
