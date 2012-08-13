#pragma once

#include <cstdint>

namespace Pacman {

typedef uint64_t resource_id_t;

class Resource
{
public:

	Resource() : mId(gIdCounter++) {}
	Resource(const Resource&) = delete;
	virtual ~Resource() {}

	Resource& operator= (const Resource&) = delete;

	resource_id_t GetId() const
	{
		return mId;
	}

private:

	resource_id_t mId;
	static resource_id_t gIdCounter;
};

} // Pacman namespace