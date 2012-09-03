#include "actor.h"
#include "error.h"
#include "scene_node.h"
#include "scene_manager.h"
#include "json_helper.h"
#include "map.h"

namespace Pacman {

static ActorMoveDirection ConvertDirection(const std::string& direction)
{
    if (direction == "left")
        return ActorMoveDirection::Left;
    if (direction == "right")
        return ActorMoveDirection::Right;
    if (direction == "up")
        return ActorMoveDirection::Up;
    if (direction == "down")
        return ActorMoveDirection::Down;
    return ActorMoveDirection::None;
}

Actor::Actor(const std::string& textData, std::shared_ptr<IDrawable> drawable,
             const uint16_t size, const std::weak_ptr<Map> map)
     : mDirection(ActorMoveDirection::None),
       mSize(size)
{
    const Json::Value root = JsonHelper::ParseJson(textData);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value startCellIndex = root["startRightCellIndex"];
    const Json::Value direction = root["direction"];
    const Json::Value speed = root["speed"];
    PACMAN_CHECK_ERROR(startCellIndex.isArray() && (startCellIndex.size() == 2) &&
                       direction.isString() && speed.isNumeric(), ErrorCode::BadFormat);

    const Json::Value cellIndexRow = startCellIndex[size_t(0)];
    const Json::Value cellIndexColumn = startCellIndex[size_t(1)];
    PACMAN_CHECK_ERROR(cellIndexRow.isNumeric() && cellIndexColumn.isNumeric(), ErrorCode::BadFormat);

    mNextDirection = ConvertDirection(direction.asString());
    mSpeed = speed.asUInt();

    CellIndex rightCellIndex(cellIndexRow.asUInt(), cellIndexColumn.asUInt());
    const std::shared_ptr<Map> mapPtr = map.lock();
    PACMAN_CHECK_ERROR(mapPtr != nullptr, ErrorCode::InvalidState);

    const uint16_t cellSize = mapPtr->GetCellSize();
    const uint16_t cellSizeHalf = cellSize / 2;
    const uint16_t actorsSizeHalf = mSize / 2;

    // find start position
    SpritePosition startPosition = mapPtr->GetCellCenterPos(rightCellIndex);
    startPosition.SetX(startPosition.GetX() - cellSizeHalf); // position between cells
    // move actors drawable center into this position
    startPosition.SetX(startPosition.GetX() - actorsSizeHalf);
    startPosition.SetY(startPosition.GetY() - actorsSizeHalf);

    mNode = std::make_shared<SceneNode>(drawable, startPosition);
}

void Actor::Translate(const SpritePosition& position)
{
    mNode->Translate(position);
}

void Actor::MoveForward(const SpritePosition& offset)
{
    mNode->MoveForward(offset);
}

void Actor::MoveBack(const SpritePosition& offset)
{
    mNode->MoveBack(offset);
}

void Actor::AttachToScene(SceneManager& sceneManager) const
{
    sceneManager.AttachNode(mNode);
}

void Actor::DetachFromScene(SceneManager& sceneManager) const
{
    sceneManager.DetachNode(mNode);
}

SpriteRegion Actor::GetRegion() const
{
    return SpriteRegion(mNode->GetPosition(), mSize, mSize);
}

} // Pacman namespace