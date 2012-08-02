#pragma once

#include "scene_node.h"
#include <list>

namespace Pacman{

class SceneManager
{
public:

	SceneManager() = delete;
	SceneManager(const float sceneWidth, const float sceneHeigth);
	SceneManager(const SceneManager&) = delete;
	~SceneManager() = default;

	SceneManager& operator= (const SceneManager&) = delete;

	void AttachNode(const SceneNode& node);

	void DetachNode(const SceneNode& node);

	const std::list<SceneNode> GetNodes() const
	{
		return mNodes;
	}

private:

	float mSceneWidth;
	float mSceneHeigth;

	std::list<SceneNode> mNodes;
};

} // Pacman namespace
