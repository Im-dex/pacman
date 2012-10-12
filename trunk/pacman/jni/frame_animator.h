#pragma once

#include <vector>
#include <memory>
#include <cstdint>

#include "base.h"
#include "engine_forwdecl.h"
#include "drawable.h"

namespace Pacman {

// TODO: try to move this functionality to a shader
class FrameAnimator : public IDrawable
{
public:

	FrameAnimator() = delete;
	// frameDuration in milliseconds
	FrameAnimator(const std::vector<std::shared_ptr<Sprite>>& frames, const uint64_t frameDuration);
	FrameAnimator(const FrameAnimator&) = delete;
	~FrameAnimator() = default;

	FrameAnimator& operator= (const FrameAnimator&) = delete;

	void Update(const uint64_t dt);

	virtual std::shared_ptr<VertexBuffer> GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual std::shared_ptr<ShaderProgram> GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

    void Pause()
    {
        mPaused = true;
    }

    void Resume()
    {
        mPaused = false;
    }

    bool IsPaused() const
    {
        return mPaused;
    }

private:

	std::vector<std::shared_ptr<Sprite>> mFrames;
	const uint64_t                       mFrameDuration;
	uint64_t                             mLastFrameSwitch;
	size_t                               mCurrentFrame;
    bool                                 mPaused;
};

} // Pacman namespace