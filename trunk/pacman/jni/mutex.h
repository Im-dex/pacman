#pragma once

#include <pthread.h>

#include "base.h"

namespace Pacman {

class Mutex 
{
public:

    Mutex();
    Mutex(const Mutex&) = delete;
    ~Mutex();

    Mutex& operator= (const Mutex&) = delete;

    void Lock();

    void Unlock();

private:

    pthread_mutex_t mMutex;
};

} // Pacman namespace