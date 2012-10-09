#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "map.h"

namespace Pacman {

enum class DotType : uint8_t;
class InstancedSprite;
class SpriteSheet;
class SceneManager;
class SceneNode;

template <typename T>
struct CustomHash;

template <>
struct CustomHash<CellIndex>
{
    size_t operator() (const CellIndex& cellIndex) const
    {
        const size_t hash1 = std::hash<CellIndex::value_t>() (GetRow(cellIndex));
        const size_t hash2 = std::hash<CellIndex::value_t>() (GetColumn(cellIndex));
        return hash1 ^ (hash2 << 1);
    }
};

class DotsGrid
{
public:

    DotsGrid() = delete;
    DotsGrid(const std::vector<DotType>& dotsInfo, const std::weak_ptr<SpriteSheet>& spritesheetPtr);
    DotsGrid(const DotsGrid&) = delete;
    ~DotsGrid() = default;

    DotsGrid& operator= (const DotsGrid&) = delete;

    void AttachToScene(SceneManager& sceneManager);

    void DetachFromScene(SceneManager& sceneManager);

    void HideDot(const CellIndex& cellIndex);

    size_t GetDotsCount() const
    {
        return mDotsIndexMap.size();
    }

    size_t GetEatenDotsCount() const
    {
        return mHiddenDotsCounts;
    }

private:

    typedef std::vector<Position> InstancesArray;
    typedef std::tuple<InstancesArray, InstancesArray> DotsInstancesTuple;
    typedef std::unordered_map<CellIndex, size_t, CustomHash<CellIndex>> DotsIndexMap; // cellIndex <-> mDotsInfo.position

    DotsInstancesTuple MakeInstances(const Size smallDotSize, const Size bigDotSize);

    void AddDotInstance(const size_t dotOrderIndex, const Size dotHalfSize, std::vector<Position>& instances);

    const CellIndex::value_t         mMapColumnsCount;
    size_t                           mHiddenDotsCounts;
    std::vector<DotType>             mDotsInfo;
    DotsIndexMap                     mDotsIndexMap;
    std::shared_ptr<InstancedSprite> mSmallDotsSprite;
    std::shared_ptr<InstancedSprite> mBigDotsSprite;
    std::shared_ptr<SceneNode>       mSmallDotsNode;
    std::shared_ptr<SceneNode>       mBigDotsNode;
};

} // Pacman namespace