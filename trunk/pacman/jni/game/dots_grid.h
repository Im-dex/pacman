#pragma once

#include <vector>
#include <cstdint>
#include <memory>

namespace Pacman {

class Map;
class InstancedSprite;
class SceneNode;
enum class DotType;

class DotsGrid
{
public:

    DotsGrid() = delete;
    DotsGrid(const std::vector<DotType>& dotsInfo, std::shared_ptr<Map> map);
    DotsGrid(const DotsGrid&) = delete;
    ~DotsGrid() = default;

    DotsGrid& operator= (const DotsGrid&) = delete;

    void AttachToScene();

private:

    std::shared_ptr<InstancedSprite> MakeSprite();

    std::vector<DotType> mDots;
    std::shared_ptr<Map> mMap;
    std::shared_ptr<SceneNode> mNode;
};

} // Pacman namespace