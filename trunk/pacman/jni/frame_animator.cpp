#include "frame_animator.h"

#include "sprite.h"

namespace Pacman {

FrameAnimator::FrameAnimator(const std::vector<std::shared_ptr<Sprite>>& frames, const uint64_t frameDuration)
			 : mFrames(frames),
			   mFrameDuration(frameDuration),
			   mLastFrameSwitch(0),
			   mCurrentFrame(0),
               mPaused(false)
{
}

void FrameAnimator::Update(const uint64_t dt)
{
    if (mPaused)
        return;

	mLastFrameSwitch += dt;
	if (mLastFrameSwitch > mFrameDuration)
	{
		const size_t switchFramesCount = mLastFrameSwitch / mFrameDuration;
		mLastFrameSwitch %= mFrameDuration;
		mCurrentFrame = (mCurrentFrame + switchFramesCount) % mFrames.size();
	}
}

std::shared_ptr<VertexBuffer> FrameAnimator::GetVertexBuffer() const
{
	return mFrames[mCurrentFrame]->GetVertexBuffer();
}

std::weak_ptr<Texture2D> FrameAnimator::GetTexture() const
{
	return mFrames[mCurrentFrame]->GetTexture();
}

std::shared_ptr<ShaderProgram> FrameAnimator::GetShaderProgram() const
{
	return mFrames[mCurrentFrame]->GetShaderProgram();
}

bool FrameAnimator::HasAlphaBlend() const
{
	return mFrames[mCurrentFrame]->HasAlphaBlend();
}

} // Pacman namespace