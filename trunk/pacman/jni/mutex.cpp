#include "mutex.h"

#include "error.h"

namespace Pacman {

Mutex::Mutex()
{
    int result = pthread_mutex_init(&mMutex, nullptr);
    PACMAN_CHECK_ERROR(result == 0, ErrorCode::MutexError);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mMutex);
}

void Mutex::Lock()
{
    int result = pthread_mutex_lock(&mMutex);
    PACMAN_CHECK_ERROR(result == 0, ErrorCode::MutexError);
}

void Mutex::Unlock()
{
    int result = pthread_mutex_unlock(&mMutex);
    PACMAN_CHECK_ERROR(result == 0, ErrorCode::MutexError);
}

} // Pacman namespace