#include "loader.h"

#include "error.h"
#include "map.h"
#include "dots_grid.h"
#include "pacman.h"
#include "engine.h"
#include "asset_manager.h"
#include "renderer.h"
#include "json_helper.h"

namespace Pacman {

struct ActorData
{
    CellIndex mCellIndex;
    uint16_t  mSpeed;
};

static ActorData ParseActorData(const Json::Value& object)
{
    const Json::Value startCellIndex = object["startCellIndex"];
    const Json::Value speed = object["speed"];
    PACMAN_CHECK_ERROR(startCellIndex.isArray() && (startCellIndex.size() == 2) &&
                       speed.isNumeric(), ErrorCode::BadFormat);

    const CellIndex startCellIndexValue(startCellIndex[size_t(0)].asUInt(),
                                        startCellIndex[size_t(1)].asUInt());
    return ActorData { startCellIndexValue, static_cast<uint16_t>(speed.asUInt()) };
}

std::shared_ptr<Map> GameLoader::LoadMap(const std::string& fileName, const uint16_t cellSize)
{
    Engine* engine = GetEngine();
    AssetManager& assetManager = engine->GetAssetManager();
    Renderer& renderer = engine->GetRenderer();
    const std::string jsonData = assetManager.LoadTextFile(fileName);

    const Json::Value root = JsonHelper::ParseJson(jsonData);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value rowsCount = root["rowsCount"];
    const Json::Value leftTunnelExit = root["leftTunnelExit"];
    const Json::Value rightTunnelExit = root["rightTunnelExit"];
    const Json::Value cells = root["cells"];
    PACMAN_CHECK_ERROR(rowsCount.isNumeric() && leftTunnelExit.isArray() &&
                       (leftTunnelExit.size() == 2) && rightTunnelExit.isArray() &&
                       (rightTunnelExit.size() == 2) && cells.isArray() &&
                       (cells.size() > 0), ErrorCode::BadFormat);

    const uint16_t rowsCountValue = static_cast<const uint16_t>(rowsCount.asUInt());
    const uint16_t leftTunnelExitRow = static_cast<const uint16_t>(leftTunnelExit[size_t(0)].asUInt());
    const uint16_t leftTunnelExitColumn = static_cast<const uint16_t>(leftTunnelExit[size_t(1)].asUInt());
    const uint16_t rightTunnelExitRow = static_cast<const uint16_t>(rightTunnelExit[size_t(0)].asUInt());
    const uint16_t rightTunnelExitColumn = static_cast<const uint16_t>(rightTunnelExit[size_t(1)].asUInt());
    PACMAN_CHECK_ERROR((cells.size() % rowsCountValue) == 0, ErrorCode::BadFormat);

    const CellIndex leftTunnelExitValue(leftTunnelExitRow, leftTunnelExitColumn);
    const CellIndex rightTunnelExitValue(rightTunnelExitRow, rightTunnelExitColumn);

    std::vector<MapCellType> cellsValues;
    mDotsInfo.clear();
    mDotsInfo.reserve(cells.size());
    cellsValues.reserve(cells.size());

    for (size_t i = 0; i < cells.size(); i++)
    {
        const Json::Value cell = cells[i];
        PACMAN_CHECK_ERROR(cell.isNumeric(), ErrorCode::BadFormat);

        uint8_t value = static_cast<uint8_t>(cell.asUInt());
        PACMAN_CHECK_ERROR(value < kCellTypesCount + kDotTypesCount, ErrorCode::BadFormat);

        DotType dot = DotType::None;
        if ((value == static_cast<uint8_t>(DotType::Small)) || (value == static_cast<uint8_t>(DotType::Big)))
        {
            dot = static_cast<DotType>(value);
            value = static_cast<uint8_t>(MapCellType::Empty);
        }

        cellsValues.push_back(static_cast<MapCellType>(value));
        mDotsInfo.push_back(dot);
    }

    return std::make_shared<Map>(cellSize, rowsCountValue, renderer.GetViewportWidth(), renderer.GetViewportHeight(),
                                 leftTunnelExitValue, rightTunnelExitValue, cellsValues);
}

std::shared_ptr<DotsGrid> GameLoader::MakeDotsGrid(const std::weak_ptr<Map> mapPtr, const std::weak_ptr<SpriteSheet> spritesheetPtr)
{
    return std::make_shared<DotsGrid>(mDotsInfo, mapPtr, spritesheetPtr);
}

std::shared_ptr<PacmanActor> GameLoader::LoadPacmanActor(const std::string& fileName, const uint16_t actorSize,
                                                         const std::weak_ptr<SpriteSheet> spriteSheet, const std::shared_ptr<Map> map) const
{
    AssetManager& assetManager = GetEngine()->GetAssetManager();
    const std::string jsonData = assetManager.LoadTextFile(fileName);

    const Json::Value root = JsonHelper::ParseJson(jsonData);
    PACMAN_CHECK_ERROR(root.isObject(), ErrorCode::BadFormat);

    const Json::Value actor = root["actor"];
    const Json::Value direction = root["direction"];
    const Json::Value animationFrameDuration = root["animationFrameDuration"];
    PACMAN_CHECK_ERROR(actor.isObject() && direction.isNumeric() &&
                       animationFrameDuration.isNumeric(), ErrorCode::BadFormat);

    const ActorData actorData = ParseActorData(actor);
    const uint8_t directionNumberValue = direction.asUInt();
    const uint64_t animationFrameDurationValue = animationFrameDuration.asUInt();
    PACMAN_CHECK_ERROR(directionNumberValue < kPacmanMoveDirectionCount, ErrorCode::BadFormat);
    const PacmanMoveDirection directionValue = static_cast<PacmanMoveDirection>(directionNumberValue);

    const uint16_t cellSize = map->GetCellSize();
    const uint16_t cellSizeHalf = cellSize / 2;
    const uint16_t actorsSizeHalf = actorSize / 2;

    // find start position
    SpritePosition startPosition = map->GetCellCenterPos(actorData.mCellIndex);
    startPosition.SetX(startPosition.GetX() - cellSizeHalf); // position between cells
    // move actors drawable center into this position
    startPosition.SetX(startPosition.GetX() - actorsSizeHalf);
    startPosition.SetY(startPosition.GetY() - actorsSizeHalf);

    return std::make_shared<PacmanActor>(actorSize, actorData.mSpeed, startPosition, cellSize, animationFrameDurationValue,
                                         directionValue, spriteSheet, map);
}

} // Pacman namespace