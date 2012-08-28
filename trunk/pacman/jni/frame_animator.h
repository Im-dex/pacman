#pragma once

#include "drawable.h"

#include <vector>
#include <memory>
#include <cstdint>

namespace Pacman {

class Sprite;

class FrameAnimator : public IDrawable
{
public:

	FrameAnimator() = delete;
	// frameDuration in milliseconds
	FrameAnimator(const std::vector<std::shared_ptr<Sprite>>& frames, const size_t frameDuration);
	FrameAnimator(const FrameAnimator&) = delete;
	~FrameAnimator() = default;

	FrameAnimator& operator= (const FrameAnimator&) = delete;

	void Update(const uint64_t dt);

	virtual VertexBuffer& GetVertexBuffer() const;

	virtual std::weak_ptr<Texture2D> GetTexture() const;

	virtual ShaderProgram& GetShaderProgram() const;

	virtual bool HasAlphaBlend() const;

private:

	std::vector<std::shared_ptr<Sprite>> mFrames;
	const size_t mFrameDuration;
	uint64_t mLastFrameSwitch;
	size_t mCurrentFrame;
};

} // Pacman namespace