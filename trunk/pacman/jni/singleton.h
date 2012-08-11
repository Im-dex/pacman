#pragma once

#include "base.h"

#include <cassert>

namespace Pacman {

template <typename T>
class Singleton
{
public:

	Singleton();
	Singleton(const Singleton<T>&) = delete;
	virtual ~Singleton();

	Singleton& operator= (const Singleton&) = delete;

	static T* GetInstance();

private:

	static T* mSelf;
};

template <typename T>
T* Singleton<T>::mSelf = nullptr;

template <typename T>
FORCEINLINE Singleton<T>::Singleton()
{
	assert(mSelf == nullptr);
	mSelf = static_cast<T*>(this);
}

template <typename T>
FORCEINLINE T* Singleton<T>::GetInstance()
{
	assert(mSelf != nullptr);
	return mSelf;
}

template <typename T>
FORCEINLINE Singleton<T>::~Singleton()
{
	mSelf = nullptr;
}

} // Pacman namespace