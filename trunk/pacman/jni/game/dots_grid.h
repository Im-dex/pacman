#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "map.h"

namespace Pacman {

enum class DotType : uint8_t;
class InstancedSprite;
class SpriteSheet;
class SceneManager;

template <typename T>
struct CustomHash;

template <>
struct CustomHash<CellIndex>
{
    size_t operator() (const CellIndex& index) const
    {
        const size_t hash1 = std::hash<decltype(index.GetX())>() (index.GetX());
        const size_t hash2 = std::hash<decltype(index.GetY())>() (index.GetY());
        return hash1 ^ (hash2 << 1);
    }
};

class DotsGrid
{
public:

    DotsGrid() = delete;
    DotsGrid(const std::vector<DotType>& dotsInfo, const std::shared_ptr<Map> map, const SpriteSheet& spritesheet);
    DotsGrid(const DotsGrid&) = delete;
    ~DotsGrid() = default;

    DotsGrid& operator= (const DotsGrid&) = delete;

    void AttachToScene(SceneManager& sceneManager);

    void HideDot(const CellIndex& index);

private:

    typedef std::vector<SpritePosition> InstancesArray;
    typedef std::tuple<InstancesArray, InstancesArray> DotsInstancesTuple;
    typedef std::unordered_map<CellIndex, uint16_t, CustomHash<CellIndex>> DotsIndexMap;

    DotsInstancesTuple MakeInstances(const std::shared_ptr<Map> map, const uint16_t smallDotSize, const uint16_t bigDotSize);

    void AddDotInstance(const uint16_t dotOrderIndex, const uint16_t dotHalfSize, const std::shared_ptr<Map> map,
                        std::vector<SpritePosition>& instances);

    DotsIndexMap                     mDotsIndexMap;
    const std::vector<DotType>       mDotsInfo;
    const uint16_t                   mMapColumnsCount;
    std::shared_ptr<InstancedSprite> mSmallDotsSprite;
    std::shared_ptr<InstancedSprite> mBigDotsSprite;
};

} // Pacman namespace