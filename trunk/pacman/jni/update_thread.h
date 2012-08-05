#pragma once

#include "scene_node.h"

#include <pthread.h>
#include <memory>

namespace Pacman {

class UpdateThread
{
public:

	UpdateThread();
	UpdateThread(const UpdateThread&) = delete;
	~UpdateThread();

	UpdateThread& operator= (const UpdateThread&) = delete;

	void Start();

	void StopJoin();

	void SetNode(std::shared_ptr<SceneNode> node)
	{
		mNode = node;
	}

	void Lock();

	void Unlock();

private:

	static void* ThreadWork(void* params);

	std::shared_ptr<SceneNode> mNode;

	pthread_t mThread;
	pthread_mutex_t mMutex;
	bool	  mIsRun;
};

} // Pacman namespace
