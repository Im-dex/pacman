#pragma once

#include "base.h"

namespace Pacman {

class Timer
{
public:

	Timer() = default;
	Timer(const Timer&)= default;
	~Timer() = default;

	Timer& operator= (const Timer&) = default;

	void Start()
	{
		mInitValue = GetCurrentTime();
	}

	// returns time in nanoseconds
	uint64_t GetNanosec() const
	{
		return GetCurrentTime() - mInitValue;
	}

	// returns time in milliseconds
	uint64_t GetMillisec() const
	{
		return GetNanosec() / 1000000;
	}

private:

	uint64_t GetCurrentTime() const;

	uint64_t mInitValue;
};

} // Pacman namespace
