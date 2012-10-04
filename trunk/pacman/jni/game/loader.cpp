#include "loader.h"

#include "error.h"
#include "map.h"
#include "dots_grid.h"
#include "actor.h"
#include "engine.h"
#include "drawable.h"
#include "asset_manager.h"
#include "renderer.h"
#include "json_helper.h"
#include "utils.h"

namespace Pacman {

static Position CalcActorPosition(const Size cellSize, const Size actorSize,
                                  const Position& startCellCenterPos)
{
    const Size cellSizeHalf = cellSize / 2;
    const Size actorsSizeHalf = actorSize / 2;
    return startCellCenterPos - Position(cellSizeHalf + actorsSizeHalf, actorsSizeHalf);
}

std::shared_ptr<Map> GameLoader::LoadMap(const std::string& fileName, const Size cellSize)
{
    Engine& engine = GetEngine();
    AssetManager& assetManager = engine.GetAssetManager();
    Renderer& renderer = engine.GetRenderer();

    const std::string jsonData = assetManager.LoadTextFile(fileName);
    const JsonHelper::Value root(jsonData);

    const JsonHelper::Array leftTunnelExit = root.GetValue<JsonHelper::Array>("leftTunnelExit");
    const JsonHelper::Array rightTunnelExit = root.GetValue<JsonHelper::Array>("rightTunnelExit");
    const JsonHelper::Array cells = root.GetValue<JsonHelper::Array>("cells");
    PACMAN_CHECK_ERROR((leftTunnelExit.GetSize() == 2) && (rightTunnelExit.GetSize() == 2) &&
                       (cells.GetSize() > 0), ErrorCode::BadFormat);

    const size_t rowsCount = root.GetValue<size_t>("rowsCount");
    PACMAN_CHECK_ERROR((cells.GetSize() % rowsCount) == 0, ErrorCode::BadFormat);

    const CellIndex leftTunnelExitValue(leftTunnelExit[0].GetAs<CellIndex::value_t>(),
                                        leftTunnelExit[1].GetAs<CellIndex::value_t>());

    const CellIndex rightTunnelExitValue(rightTunnelExit[0].GetAs<CellIndex::value_t>(),
                                         rightTunnelExit[1].GetAs<CellIndex::value_t>());

    std::vector<MapCellType> cellsValues;
    mDotsInfo.clear();
    mDotsInfo.reserve(cells.GetSize());
    cellsValues.reserve(cells.GetSize());

    for (const JsonHelper::Value& cell : cells)
    {
        typedef EnumType<DotType>::value DotTypeValueT;
        DotTypeValueT value = cell.GetAs<DotTypeValueT>();

        DotType dot = DotType::None;
        if ((value == EnumCast(DotType::Small)) || (value == EnumCast(DotType::Big)))
        {
            dot = MakeEnum<DotType>(value);
            value = EnumCast(MapCellType::Empty);
        }

        cellsValues.push_back(MakeEnum<MapCellType>(value));
        mDotsInfo.push_back(dot);
    }

    return std::make_shared<Map>(cellSize, rowsCount, renderer.GetViewportWidth(), renderer.GetViewportHeight(),
                                 leftTunnelExitValue, rightTunnelExitValue, cellsValues);
}

std::shared_ptr<DotsGrid> GameLoader::MakeDotsGrid(const std::weak_ptr<Map>& mapPtr, const std::weak_ptr<SpriteSheet>& spritesheetPtr)
{
    return std::make_shared<DotsGrid>(mDotsInfo, mapPtr, spritesheetPtr);
}

std::shared_ptr<Actor> GameLoader::LoadActor(const std::string& fileName, const Size actorSize,
                                             const std::shared_ptr<IDrawable>& drawable, const std::shared_ptr<Map>& map) const
{
    typedef EnumType<MoveDirection>::value MoveDirectionValueT;
    AssetManager& assetManager = GetEngine().GetAssetManager();

    const std::string jsonData = assetManager.LoadTextFile(fileName);
    const JsonHelper::Value root(jsonData);

    const JsonHelper::Array startCellIndexArray = root.GetValue<JsonHelper::Array>("startCellIndex");
    PACMAN_CHECK_ERROR(startCellIndexArray.GetSize() == 2, ErrorCode::BadFormat);

    const CellIndex startCellIndex(startCellIndexArray[0].GetAs<CellIndex::value_t>(),
                                   startCellIndexArray[1].GetAs<CellIndex::value_t>());
    const MoveDirectionValueT startDirection = root.GetValue<MoveDirectionValueT>("startDirection");
    const Size startWayLenght = root.GetValue<Size>("startWayLenght");
    const Speed startSpeed = root.GetValue<Speed>("startSpeed");

    const Size cellSize = map->GetCellSize();
    const Position startPosition = CalcActorPosition(cellSize, actorSize, map->GetCellCenterPos(startCellIndex));

    const std::shared_ptr<Actor> actor = std::make_shared<Actor>(actorSize, startSpeed, cellSize, startPosition, drawable, map);
    actor->Move(MakeEnum<MoveDirection>(startDirection), startWayLenght);
    return actor;
}

} // Pacman namespace