#include "ghost.h"

#include "error.h"
#include "sprite.h"
#include "spritesheet.h"
#include "scene_node.h"

namespace Pacman {

static std::shared_ptr<Sprite> MakeLookSprite(const std::weak_ptr<SpriteSheet> spriteSheetPtr, const std::string& spriteName,
                                              const uint16_t actorSize)
{
    const std::shared_ptr<SpriteSheet> spriteSheet = spriteSheetPtr.lock();
    PACMAN_CHECK_ERROR(spriteSheet != nullptr, ErrorCode::BadArgument);
    return spriteSheet->MakeSprite(spriteName, SpriteRegion(0, 0, actorSize, actorSize));
}

GhostActor::GhostActor(const uint16_t size, const uint16_t speed, const SpritePosition& startPosition,
                       const uint16_t cellSize, const std::weak_ptr<SpriteSheet> spriteSheet,
                       const std::string& leftSpriteName, const std::string& rightSpriteName,
                       const std::string& topSpriteName, const std::string& bottomSpriteName)
     : Actor(size, speed, startPosition, cellSize, nullptr),
       mLeftSprite(MakeLookSprite(spriteSheet, leftSpriteName, size)),
       mRightSprite(MakeLookSprite(spriteSheet, rightSpriteName, size)),
       mTopSprite(MakeLookSprite(spriteSheet, topSpriteName, size)),
       mBottomSprite(MakeLookSprite(spriteSheet, bottomSpriteName, size))
{
    Actor::mNode->SetDrawable(mLeftSprite);
    //MoveTo();
}

} // Pacman namespace