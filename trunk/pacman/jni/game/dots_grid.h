#pragma once

#include <vector>
#include <cstdint>
#include <memory>

namespace Pacman {

enum class DotType : uint8_t;
class Map;
class InstancedSprite;
class SpriteSheet;
class SceneManager;

class DotsGrid
{
public:

    DotsGrid() = delete;
    DotsGrid(const std::vector<DotType>& dotsInfo, const std::shared_ptr<Map> map, const SpriteSheet& spritesheet);
    DotsGrid(const DotsGrid&) = delete;
    ~DotsGrid() = default;

    DotsGrid& operator= (const DotsGrid&) = delete;

    void AttachToScene(SceneManager& sceneManager);

private:

    std::vector<DotType> mDots;
    std::shared_ptr<Map> mMap;
    std::shared_ptr<InstancedSprite> mSmallDotsSprite;
    std::shared_ptr<InstancedSprite> mBigDotsSprite;
};

} // Pacman namespace