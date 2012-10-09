#include "loader.h"

#include "error.h"
#include "game.h"
#include "map.h"
#include "dots_grid.h"
#include "actor.h"
#include "engine.h"
#include "drawable.h"
#include "asset_manager.h"
#include "renderer.h"
#include "ai_controller.h"
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

std::unique_ptr<Map> GameLoader::LoadMap(const std::string& fileName, const Size cellSize)
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

    return std::unique_ptr<Map>(new Map(cellSize, rowsCount, renderer.GetViewportWidth(), renderer.GetViewportHeight(),
                                        leftTunnelExitValue, rightTunnelExitValue, cellsValues));
}

std::unique_ptr<DotsGrid> GameLoader::MakeDotsGrid(const std::weak_ptr<SpriteSheet>& spritesheetPtr)
{
    return std::unique_ptr<DotsGrid>(new DotsGrid(mDotsInfo, spritesheetPtr));
}

std::shared_ptr<Actor> GameLoader::LoadActor(const std::string& fileName, const Size actorSize,
                                             const std::shared_ptr<IDrawable>& drawable) const
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
    const Speed startSpeed = root.GetValue<Speed>("startSpeed");

    Map& map = GetGame().GetMap();
    const Size cellSize = map.GetCellSize();
    const Position startPosition = CalcActorPosition(cellSize, actorSize, map.GetCellCenterPos(startCellIndex));

    return std::make_shared<Actor>(actorSize, startSpeed, startPosition, MakeEnum<MoveDirection>(startDirection), drawable);
}

AIInfo GameLoader::LoadAIInfo(const std::string& fileName) const
{
    AssetManager& assetManager = GetEngine().GetAssetManager();

    const std::string jsonData = assetManager.LoadTextFile(fileName);
    const JsonHelper::Value root(jsonData);

    const JsonHelper::Value scatterTarget = root.GetValue<JsonHelper::Value>("scatter_target");
    const JsonHelper::Array blinkyScatterTarget = scatterTarget.GetValue<JsonHelper::Array>("blinky");
    const JsonHelper::Array pinkyScatterTarget = scatterTarget.GetValue<JsonHelper::Array>("pinky");
    const JsonHelper::Array inkyScatterTarget = scatterTarget.GetValue<JsonHelper::Array>("inky");
    const JsonHelper::Array clydeScatterTarget = scatterTarget.GetValue<JsonHelper::Array>("clyde");
    PACMAN_CHECK_ERROR((blinkyScatterTarget.GetSize() == 2) &&
                        (pinkyScatterTarget.GetSize() == 2) &&
                        (inkyScatterTarget.GetSize() == 2) &&
                        (clydeScatterTarget.GetSize() == 2), ErrorCode::BadFormat);

    std::vector<ChaseDirectionDiscard> discardCells;
    const JsonHelper::Array chaseDirectionDiscard = root.GetValue<JsonHelper::Array>("chase_direction_discard");
    for (const JsonHelper::Value& value : chaseDirectionDiscard)
    {
        typedef EnumType<MoveDirection>::value MoveDirectionValueT;
        const JsonHelper::Array tuple = value.GetAs<JsonHelper::Array>();
        const JsonHelper::Array cell = tuple[0].GetAs<JsonHelper::Array>();
        const MoveDirectionValueT direction = tuple[1].GetAs<MoveDirectionValueT>();

        discardCells.push_back(ChaseDirectionDiscard{
            CellIndex(cell[0].GetAs<CellIndex::value_t>(),
                      cell[1].GetAs<CellIndex::value_t>()),
            MakeEnum<MoveDirection>(direction)
        });
    }

    return AIInfo
    {
        CellIndex(blinkyScatterTarget[0].GetAs<CellIndex::value_t>(),
                  blinkyScatterTarget[1].GetAs<CellIndex::value_t>()),
        CellIndex(pinkyScatterTarget[0].GetAs<CellIndex::value_t>(),
                  pinkyScatterTarget[1].GetAs<CellIndex::value_t>()),
        CellIndex(inkyScatterTarget[0].GetAs<CellIndex::value_t>(),
                  inkyScatterTarget[1].GetAs<CellIndex::value_t>()),
        CellIndex(clydeScatterTarget[0].GetAs<CellIndex::value_t>(),
                  clydeScatterTarget[1].GetAs<CellIndex::value_t>()),
        root.GetValue<uint64_t>("scatter_duration"),
        root.GetValue<uint64_t>("scatter_interval"),
        discardCells,
        root.GetValue<uint64_t>("fright_duration")
    };
}

} // Pacman namespace