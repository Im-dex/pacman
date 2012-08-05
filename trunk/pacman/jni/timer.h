#pragma once

#include <cstdint>

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
	uint64_t GetNanosec()
	{
		return GetCurrentTime() - mInitValue;
	}

	// returns time in milliseconds
	uint64_t GetMillisec()
	{
		return GetNanosec() / 1000000;
	}

private:

	uint64_t GetCurrentTime();

	uint64_t mInitValue;
};

} // Pacman namespace
