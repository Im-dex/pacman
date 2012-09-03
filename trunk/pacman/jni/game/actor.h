#pragma once

#include <memory>

#include "engine_typedefs.h"

namespace Pacman {

class SceneNode;
class IDrawable;
class SceneManager;
class Map;

enum class ActorMoveDirection
{
    None,
    Left,
    Right,
    Up,
    Down
};

class Actor
{
public:

    Actor() = delete;
    Actor(const std::string& textData, std::shared_ptr<IDrawable> drawable,
          const uint16_t size, const std::weak_ptr<Map> map);
    Actor(const Actor&) = default;
    virtual ~Actor() {}

    Actor& operator= (const Actor&) = default;

    void Translate(const SpritePosition& position);

    void MoveForward(const SpritePosition& offset);

    void MoveBack(const SpritePosition& offset);

    void AttachToScene(SceneManager& sceneManager) const;

    void DetachFromScene(SceneManager& sceneManager) const;

    SpriteRegion GetRegion() const;

    ActorMoveDirection GetDirection() const
    {
        return mDirection;
    }

    ActorMoveDirection GetNextDirection() const
    {
        return mNextDirection;
    }

    uint16_t GetSpeed() const
    {
        return mSpeed;
    }

    void SetDirection(const ActorMoveDirection direction)
    {
        mDirection = direction;
    }

    void SetNextDirection(const ActorMoveDirection direction)
    {
        mNextDirection = direction;
    }

private:

    const uint16_t             mSize;
    ActorMoveDirection         mDirection;
    ActorMoveDirection         mNextDirection;
    uint16_t                   mSpeed;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace