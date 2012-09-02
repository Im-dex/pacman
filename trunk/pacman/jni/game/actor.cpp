#include "actor.h"
#include "error.h"
#include "scene_node.h"
#include "json_helper.h"

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

Actor::Actor(const std::string& textData, std::shared_ptr<SceneNode> node)
     : mNextDirection(ActorMoveDirection::None),
       mNode(node)
{
    const Json::Value root = JsonHelper::ParseJson(textData);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value startCellIndex = root["startCellIndex"];
    const Json::Value direction = root["direction"];
    const Json::Value speed = root["speed"];
    PACMAN_CHECK_ERROR(startCellIndex.isArray() && (startCellIndex.size() == 2) &&
                       direction.isString() && speed.isNumeric(), ErrorCode::BadFormat);

    const Json::Value cellIndexX = startCellIndex[size_t(0)];
    const Json::Value cellIndexY = startCellIndex[size_t(1)];
    PACMAN_CHECK_ERROR(cellIndexX.isNumeric() && cellIndexY.isNumeric(), ErrorCode::BadFormat);

    mMapCellIndex = CellIndex(cellIndexX.asUInt(), cellIndexY.asUInt());
    mDirection = ConvertDirection(direction.asString());
    mSpeed = speed.asUInt();
}

Actor::Actor(std::shared_ptr<SceneNode> node, const CellIndex& startIndex,
             const ActorMoveDirection startDirection, const size_t moveSpeed)
     : mMapCellIndex(startIndex),
       mDirection(startDirection),
       mNextDirection(ActorMoveDirection::None),
       mSpeed(moveSpeed),
       mNode(node)
{
}

void Actor::Translate(const Math::Vector2f& position)
{
    mNode->Translate(position);
}

void Actor::Move(const Math::Vector2f& offset)
{
    mNode->Move(offset);
}

} // Pacman namespace