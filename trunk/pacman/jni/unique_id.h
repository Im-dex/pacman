#pragma once

#include <cstdint>

namespace Pacman {

typedef uint64_t uid_t;

class UniqueIdProvider
{
public:

	static const uid_t kNull;

	UniqueIdProvider() : mId(gIdCounter++) {}
	UniqueIdProvider(const UniqueIdProvider&) = default;
	virtual ~UniqueIdProvider() {}

	UniqueIdProvider& operator= (const UniqueIdProvider&) = default;

	uid_t GetId() const
	{
		return mId;
	}

private:

	uid_t mId;
	static uid_t gIdCounter;
};

} // Pacman namespace
