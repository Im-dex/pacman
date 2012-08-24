#pragma once

#include <list>
#include <memory>

namespace Pacman{

class SceneNode;

class SceneManager
{
public:

	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	~SceneManager() = default;

	SceneManager& operator= (const SceneManager&) = delete;

	void AttachNode(std::shared_ptr<SceneNode> node);

	void DetachNode(std::shared_ptr<SceneNode> node);

	std::list<std::shared_ptr<SceneNode>> GetNodes() const
	{
		return mNodes;
	}

private:

	float mSceneWidth;
	float mSceneHeigth;

	std::list<std::shared_ptr<SceneNode>> mNodes;
};

} // Pacman namespace
