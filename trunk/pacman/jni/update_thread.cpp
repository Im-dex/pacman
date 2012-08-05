#include "update_thread.h"
#include "error.h"

#include "math/vector2.h"
#include "timer.h"

#include "base.h"

#include <unistd.h>

namespace Pacman {

static const float speed = 100; // pixels per second

static const uint64_t kMinUpdateIntervalMillisec = 16;

UpdateThread::UpdateThread()
			: mThread(),
			  mIsRun(false)
{
	int res = pthread_mutex_init(&mMutex, nullptr);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::MutexError);
}

UpdateThread::~UpdateThread()
{
	if (mIsRun)
	{
		StopJoin();
	}

	int res = pthread_mutex_destroy(&mMutex);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::MutexError);
}

void UpdateThread::Start()
{
	mIsRun = true;
	int res = pthread_create(&mThread, nullptr, &UpdateThread::ThreadWork, static_cast<void*>(this));
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::CreateThreadFailed);
}

void UpdateThread::StopJoin()
{
	mIsRun = false;
	int res = pthread_join(mThread, nullptr);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::JoinThreadFailed);
}

void UpdateThread::Lock()
{
	int res = pthread_mutex_lock(&mMutex);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::MutexError);
}

void UpdateThread::Unlock()
{
	int res = pthread_mutex_unlock(&mMutex);
	PACMAN_CHECK_ERROR(res == 0, ErrorCode::MutexError);
}

void* UpdateThread::ThreadWork(void* params)
{
	UpdateThread* This = static_cast<UpdateThread*>(params);

	Timer timer;
	timer.Start();

	uint64_t startTime = timer.GetMillisec();

	while (This->mIsRun)
	{
		uint64_t dt = timer.GetMillisec() - startTime;

		LOGI("DT: %llu", dt);

		if (dt >= kMinUpdateIntervalMillisec)
		{
			This->Lock();

			Math::Vector2f pos = This->mNode->GetPosition();
			if (pos.GetX() >= 800.0f)
			{
				pos.SetX(0.0f);
				This->mNode->Translate(pos);
			}

			const float offset_x = (speed * dt) / 1000.0f;
			LOGI("offset by time: %f", offset_x);
			This->mNode->Move(Math::Vector2f(offset_x, 0.0f));
			startTime = timer.GetMillisec();

			This->Unlock();
		}
		else
		{
			usleep(0);
		}
	}

	return nullptr;
}

} // Pacman namespace
