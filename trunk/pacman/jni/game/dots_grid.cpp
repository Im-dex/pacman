#include "dots_grid.h"

#include <string>
#include <vector>
#include <limits>

#include "error.h"
#include "instanced_sprite.h"
#include "spritesheet.h"
#include "shader_program.h"
#include "scene_node.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"

namespace Pacman {

static const std::string kDotSpriteName = "dot";

static FORCEINLINE CellIndex GetDotIndex(const uint16_t dotOrderIndex, const std::shared_ptr<Map> map)
{
    return CellIndex(dotOrderIndex / map->GetColumnsCount(), dotOrderIndex % map->GetColumnsCount());
}

static FORCEINLINE SpritePosition GetDotPosition(const CellIndex& index, const uint16_t dotSizeHalf, const std::shared_ptr<Map> map)
{
    return map->GetCellCenterPos(index) - SpritePosition(dotSizeHalf, dotSizeHalf);
}

static FORCEINLINE uint16_t GetMapColumnsCount(const std::weak_ptr<Map> mapPtr)
{
    const std::shared_ptr<Map> map = mapPtr.lock();
    PACMAN_CHECK_ERROR(map != nullptr, ErrorCode::BadArgument);
    return map->GetColumnsCount();
}

DotsGrid::DotsGrid(const std::vector<DotType>& dotsInfo, const std::weak_ptr<Map> mapPtr, const std::weak_ptr<SpriteSheet> spritesheetPtr)
        : mDotsInfo(dotsInfo),
          mMapColumnsCount(GetMapColumnsCount(mapPtr))
{
    const std::shared_ptr<Map> map = mapPtr.lock();
    const std::shared_ptr<SpriteSheet> spritesheet = spritesheetPtr.lock();
    PACMAN_CHECK_ERROR((map != nullptr) && (spritesheet != nullptr), ErrorCode::BadArgument);
    PACMAN_CHECK_ERROR(dotsInfo.size() < std::numeric_limits<uint32_t>::max(), ErrorCode::BadArgument);

    AssetManager& assetManager = GetEngine()->GetAssetManager();

    const uint16_t smallDotSize = map->GetCellSize() / 2;
    const uint16_t bigDotSize = map->GetCellSize();
    SpriteRegion smallRegion(0, 0, smallDotSize, smallDotSize);
    SpriteRegion bigRegion(0, 0, bigDotSize, bigDotSize);

    SpriteInfo info = spritesheet->GetSpriteInfo(kDotSpriteName);
    std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(info.mVertexShaderName, info.mFragmentShaderName);

    DotsInstancesTuple instancesTuple = MakeInstances(map, smallDotSize, bigDotSize);
    std::vector<SpritePosition>& smallDotsInstances = std::get<0>(instancesTuple);
    std::vector<SpritePosition>& bigDotsInstances = std::get<1>(instancesTuple);

    mSmallDotsSprite = std::make_shared<InstancedSprite>(smallRegion, info.mTextureRegion, spritesheet->GetTexture(), shaderProgram,
                                                         info.mAlphaBlend, smallDotsInstances, true);

    mBigDotsSprite = std::make_shared<InstancedSprite>(bigRegion, info.mTextureRegion, spritesheet->GetTexture(), shaderProgram,
                                                       info.mAlphaBlend, bigDotsInstances, true);

    mSmallDotsNode = std::make_shared<SceneNode>(mSmallDotsSprite, SpritePosition::kZero, SpritePosition::kZero, Rotation::kZero);
    mBigDotsNode = std::make_shared<SceneNode>(mBigDotsSprite, SpritePosition::kZero, SpritePosition::kZero, Rotation::kZero);
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

void DotsGrid::HideDot(const CellIndex& index)
{
    const auto iter = mDotsIndexMap.find(index);
    if (iter == mDotsIndexMap.end())
        return;

    const DotType dotType = mDotsInfo[index.GetY() * mMapColumnsCount + index.GetX()];
    switch (dotType)
    {
    case DotType::Small:
        mSmallDotsSprite->HideInstance(iter->second);
        break;
    case DotType::Big:
        mBigDotsSprite->HideInstance(iter->second);
        break;
    }
}

DotsGrid::DotsInstancesTuple DotsGrid::MakeInstances(const std::shared_ptr<Map> map, const uint16_t smallDotSize, const uint16_t bigDotSize)
{
    const uint16_t smallDotSizeHalf = smallDotSize / 2;
    const uint16_t bigDotSizeHalf = bigDotSize / 2;

    std::vector<SpritePosition> smallDotsInstances;
    std::vector<SpritePosition> bigDotsInstances;

    for (uint16_t i = 0; i < mDotsInfo.size(); i++)
    {
        switch (mDotsInfo[i])
        {
        case DotType::Small:
            AddDotInstance(i, smallDotSizeHalf, map, smallDotsInstances);
            break;
        case DotType::Big:
            AddDotInstance(i, bigDotSizeHalf, map, bigDotsInstances);
            break;
        }
    }

    smallDotsInstances.reserve(smallDotsInstances.size());
    bigDotsInstances.reserve(bigDotsInstances.size());

    return std::make_tuple(smallDotsInstances, bigDotsInstances);
}

void DotsGrid::AddDotInstance(const uint16_t dotOrderIndex, const uint16_t dotHalfSize, const std::shared_ptr<Map> map,
                              std::vector<SpritePosition>& instances)
{
    const CellIndex index = GetDotIndex(dotOrderIndex, map);
    instances.push_back(GetDotPosition(index, dotHalfSize, map));
    mDotsIndexMap.insert(std::make_pair(index, instances.size() - 1));
}

} // Pacman namespace