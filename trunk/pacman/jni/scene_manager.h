#pragma once

#include <list>
#include <memory>

#include "base.h"
#include "engine_forwdecl.h"

namespace Pacman{

class SceneManager
{
public:

	SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	~SceneManager() = default;

	SceneManager& operator= (const SceneManager&) = delete;

	void AttachNode(const std::shared_ptr<SceneNode> node);

	void DetachNode(const std::shared_ptr<SceneNode> node);

private:

    typedef std::list<std::shared_ptr<SceneNode>> NodesList;

	 NodesList mNodes;

// range-based for loop support
public:

    NodesList::const_iterator begin() const
    {
        return mNodes.cbegin();
    }

    NodesList::const_iterator end() const
    {
        return mNodes.cend();
    }
};

} // Pacman namespace
