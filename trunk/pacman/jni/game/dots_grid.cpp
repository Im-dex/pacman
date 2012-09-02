#include "dots_grid.h"

#include <string>
#include <vector>
#include <limits>
#include <tuple>

#include "error.h"
#include "map.h"
#include "instanced_sprite.h"
#include "spritesheet.h"
#include "shader_program.h"
#include "scene_node.h"
#include "engine.h"
#include "asset_manager.h"
#include "scene_manager.h"

namespace Pacman {

typedef std::vector<SpritePosition> InstancesArray;
typedef std::tuple<InstancesArray, InstancesArray> DotsInstancesTuple;

static const std::string kDotSpriteName = "dot";

static FORCEINLINE SpritePosition GetDotPosition(const uint16_t dotOrderIndex, const uint16_t dotSizeHalf, const std::shared_ptr<Map> map)
{
    const uint16_t rowIndex = dotOrderIndex % map->GetColumnsCount();
    const uint16_t columnIndex = dotOrderIndex / map->GetColumnsCount();
    return map->GetCellCenterPos(rowIndex, columnIndex) - SpritePosition(dotSizeHalf, dotSizeHalf);
}

static DotsInstancesTuple MakeInstances(const std::vector<DotType>& dotsInfo, const uint16_t smallDotSize, const std::shared_ptr<Map> map)
{
    const uint16_t smallDotSizeHalf = smallDotSize / 2;
    const uint16_t bigDotSizeHalf = map->GetCellSize() / 2;

    std::vector<SpritePosition> smallDotsInstances;
    std::vector<SpritePosition> bigDotsInstances;
   
    for (uint16_t i = 0; i < dotsInfo.size(); i++)
    {
        switch (dotsInfo[i])
        {
        case DotType::Small:
            smallDotsInstances.push_back(GetDotPosition(i, smallDotSizeHalf, map));
            break;
        case DotType::Big:
            bigDotsInstances.push_back(GetDotPosition(i, bigDotSizeHalf, map));
            break;
        }
    }

    smallDotsInstances.reserve(smallDotsInstances.size());
    bigDotsInstances.reserve(bigDotsInstances.size());
    
    return std::make_tuple(smallDotsInstances, bigDotsInstances);
}

DotsGrid::DotsGrid(const std::vector<DotType>& dotsInfo, const std::shared_ptr<Map> map, const SpriteSheet& spritesheet)
        : mDots(dotsInfo),
          mMap(map)
{
    PACMAN_CHECK_ERROR(dotsInfo.size() < std::numeric_limits<uint32_t>::max(), ErrorCode::BadArgument);

    AssetManager& assetManager = GetEngine()->GetAssetManager();

    const uint16_t smallDotSize = map->GetCellSize() / 2;
    SpriteRegion smallRegion(0, 0, smallDotSize, smallDotSize);
    SpriteRegion bigRegion(0, 0, map->GetCellSize(), map->GetCellSize());

    SpriteInfo info = spritesheet.GetSpriteInfo(kDotSpriteName);
    std::shared_ptr<ShaderProgram> shaderProgram = assetManager.LoadShaderProgram(info.mVertexShaderName, info.mFragmentShaderName);

    DotsInstancesTuple instancesTuple = MakeInstances(dotsInfo, smallDotSize, map);
    std::vector<SpritePosition>& smallDotsInstances = std::get<0>(instancesTuple);
    std::vector<SpritePosition>& bigDotsInstances = std::get<1>(instancesTuple);

    mSmallDotsSprite = std::make_shared<InstancedSprite>(smallRegion, info.mTextureRegion, spritesheet.GetTexture(), shaderProgram,
                                                         info.mAlphaBlend, smallDotsInstances, true);

    mBigDotsSprite = std::make_shared<InstancedSprite>(bigRegion, info.mTextureRegion, spritesheet.GetTexture(), shaderProgram,
                                                       info.mAlphaBlend, bigDotsInstances, true);
}

void DotsGrid::AttachToScene(SceneManager& sceneManager)
{
    sceneManager.AttachNode(std::make_shared<SceneNode>(mSmallDotsSprite, Math::Vector2f::kZero));
    sceneManager.AttachNode(std::make_shared<SceneNode>(mBigDotsSprite, Math::Vector2f::kZero));
}

} // Pacman namespace