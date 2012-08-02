#include "scene_manager.h"
#include "error.h"

#include <algorithm>

namespace Pacman{

const size_t mSceneTreeDepth = 3;

SceneManager::SceneManager(const float sceneWidth, const float sceneHeigth)
			: mSceneWidth(sceneWidth),
			  mSceneHeigth(sceneHeigth),
			  mNodes()
{
}

void SceneManager::AttachNode(const SceneNode& node)
{
	mNodes.push_back(node);
}

void SceneManager::DetachNode(const SceneNode& node)
{
	auto iter = std::find_if(mNodes.begin(), mNodes.end(), [&node](const SceneNode& searchNode)
	{
		return searchNode.GetId() == node.GetId();
	});

	PACMAN_CHECK_ERROR(iter != mNodes.end(), ErrorCode::BadArgument);
	mNodes.erase(iter);
}

} // Pacman namespace
