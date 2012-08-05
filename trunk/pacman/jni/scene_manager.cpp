#include "scene_manager.h"
#include "error.h"
#include "scene_node.h"

#include <algorithm>

namespace Pacman{

const size_t mSceneTreeDepth = 3;

SceneManager::SceneManager(const float sceneWidth, const float sceneHeigth)
			: mSceneWidth(sceneWidth),
			  mSceneHeigth(sceneHeigth),
			  mNodes()
{
}

void SceneManager::AttachNode(std::shared_ptr<SceneNode> node)
{
	mNodes.push_back(node);
}

void SceneManager::DetachNode(std::shared_ptr<SceneNode> node)
{
	mNodes.remove(node);
}

} // Pacman namespace
