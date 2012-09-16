#pragma once

#include <memory>

#include "engine_typedefs.h"

namespace Pacman {

class SceneNode;
class IDrawable;
class SceneManager;

class Actor
{
public:

    Actor() = delete;
    Actor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
          const uint16_t cellSize, std::shared_ptr<IDrawable> drawable);
    Actor(const Actor&) = default;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = default;

    void AttachToScene(SceneManager& sceneManager) const;

    void DetachFromScene(SceneManager& sceneManager) const;

    void Update(const uint64_t dt);

    SpritePosition GetPosition() const;

    SpriteRegion GetRegion() const;

    void Rotate(const Rotation& rotation);

    void Translate(const SpritePosition& position);

    void MoveTo(const SpritePosition& position)
    {
        mMovePoint = position;
    }

protected:

    const uint16_t             mSize;
    const uint16_t             mSpeed;
    const uint16_t             mCellSize;
    SpritePosition             mMovePoint;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace