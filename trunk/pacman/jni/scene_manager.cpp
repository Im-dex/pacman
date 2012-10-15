#include "scene_manager.h"

#include <algorithm>

#include "error.h"
#include "scene_node.h"

namespace Pacman{

void SceneManager::AttachNode(const std::shared_ptr<SceneNode> node)
{
    mNodes.emplace_back(std::move(node));
}

void SceneManager::DetachNode(const std::shared_ptr<SceneNode> node)
{
	mNodes.remove(node);
}

} // Pacman namespace
