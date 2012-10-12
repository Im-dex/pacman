#include "dots_grid.h"

#include <string>
#include <vector>
#include <limits>

#include "error.h"
#include "game.h"
#include "map.h"
#include "ai_controller.h"
#include "instanced_sprite.h"
#include "spritesheet.h"
#include "shader_program.h"
#include "scene_node.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"

namespace Pacman {

static const std::string kDotSpriteName = "dot";

static FORCEINLINE CellIndex GetDotIndex(const size_t dotOrderIndex)
{
    Map& map = GetGame().GetMap();
    return CellIndex(dotOrderIndex / map.GetColumnsCount(), dotOrderIndex % map.GetColumnsCount());
}

static FORCEINLINE Position GetDotPosition(const CellIndex& cellIndex, const Size dotSizeHalf)
{
    return GetGame().GetMap().GetCellCenterPos(cellIndex) - Position(dotSizeHalf, dotSizeHalf);
}

DotsGrid::DotsGrid(const std::vector<DotType>& dotsInfo, const std::weak_ptr<SpriteSheet>& spritesheetPtr)
        : mDotsInfo(dotsInfo),
          mMapColumnsCount(GetGame().GetMap().GetColumnsCount()),
          mHiddenDotsCounts(0)
{
    const std::shared_ptr<SpriteSheet> spritesheet = spritesheetPtr.lock();
    PACMAN_CHECK_ERROR(spritesheet != nullptr);

    Map& map = GetGame().GetMap();
    AssetManager& assetManager = GetEngine().GetAssetManager();

    const Size smallDotSize = map.GetCellSize() / 2;
    const Size bigDotSize = map.GetCellSize();
    SpriteRegion smallRegion(0, 0, smallDotSize, smallDotSize);
    SpriteRegion bigRegion(0, 0, bigDotSize, bigDotSize);

    SpriteInfo info = spritesheet->GetSpriteInfo(kDotSpriteName);
    std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(info.mVertexShaderName, info.mFragmentShaderName);

    DotsInstancesTuple instancesTuple = MakeInstances(smallDotSize, bigDotSize);
    std::vector<Position>& smallDotsInstances = std::get<0>(instancesTuple);
    std::vector<Position>& bigDotsInstances = std::get<1>(instancesTuple);

    mSmallDotsSprite = std::make_shared<InstancedSprite>(smallRegion, info.mTextureRegion, spritesheet->GetTexture(), shaderProgram,
                                                         info.mAlphaBlend, smallDotsInstances, true);

    mBigDotsSprite = std::make_shared<InstancedSprite>(bigRegion, info.mTextureRegion, spritesheet->GetTexture(), shaderProgram,
                                                       info.mAlphaBlend, bigDotsInstances, true);

    mSmallDotsNode = std::make_shared<SceneNode>(mSmallDotsSprite, Position::kZero, Rotation::kZero);
    mBigDotsNode = std::make_shared<SceneNode>(mBigDotsSprite, Position::kZero, Rotation::kZero);
}

void DotsGrid::AttachToScene(SceneManager& sceneManager)
{
    sceneManager.AttachNode(mSmallDotsNode);
    sceneManager.AttachNode(mBigDotsNode);
}

void DotsGrid::DetachFromScene(SceneManager& sceneManager)
{
    sceneManager.DetachNode(mSmallDotsNode);
    sceneManager.DetachNode(mBigDotsNode);
}

void DotsGrid::HideDot(const CellIndex& cellIndex)
{
    const auto iter = mDotsIndexMap.find(cellIndex);
    if (iter == mDotsIndexMap.end())
        return;

    const size_t dotIndex = GetRow(cellIndex) * mMapColumnsCount + GetColumn(cellIndex);
    const DotType dotType = mDotsInfo[dotIndex];
    switch (dotType)
    {
    case DotType::Small:
        mSmallDotsSprite->EraseInstance(iter->second);
        mDotsInfo[dotIndex] = DotType::None;
        mHiddenDotsCounts++;
        break;
    case DotType::Big:
        mBigDotsSprite->EraseInstance(iter->second);
        mDotsInfo[dotIndex] = DotType::None;
        mHiddenDotsCounts++;
        GetGame().GetAIController().EnableFrightenedState();
        break;
    }
}

DotsGrid::DotsInstancesTuple DotsGrid::MakeInstances(const Size smallDotSize, const Size bigDotSize)
{
    const Size smallDotSizeHalf = smallDotSize / 2;
    const Size bigDotSizeHalf = bigDotSize / 2;

    std::vector<Position> smallDotsInstances;
    std::vector<Position> bigDotsInstances;

    for (size_t i = 0; i < mDotsInfo.size(); i++)
    {
        switch (mDotsInfo[i])
        {
        case DotType::Small:
            AddDotInstance(i, smallDotSizeHalf, smallDotsInstances);
            break;
        case DotType::Big:
            AddDotInstance(i, bigDotSizeHalf, bigDotsInstances);
            break;
        }
    }

    smallDotsInstances.reserve(smallDotsInstances.size());
    bigDotsInstances.reserve(bigDotsInstances.size());

    return std::make_tuple(smallDotsInstances, bigDotsInstances);
}

void DotsGrid::AddDotInstance(const size_t dotOrderIndex, const Size dotHalfSize, std::vector<Position>& instances)
{
    const CellIndex index = GetDotIndex(dotOrderIndex);
    instances.push_back(GetDotPosition(index, dotHalfSize));
    mDotsIndexMap.insert(std::make_pair(index, instances.size() - 1));
}

} // Pacman namespace