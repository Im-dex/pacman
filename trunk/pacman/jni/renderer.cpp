#include "renderer.h"

#include <GLES2/gl2.h>

#include "error.h"
#include "engine.h"
#include "scene_manager.h"
#include "scene_node.h"
#include "drawable.h"
#include "texture.h"
#include "shader_program.h"
#include "vertex_buffer.h"

namespace Pacman {

static const char* kProjectionUniformName = "mProjectionMatrix";
static const char* kModelMatrixUniformName = "mModelMatrix";
static const char* kModelProjMatrixUniformName = "mModelProjectionMatrix";

Renderer::Renderer()
		: mProjection(),
		  mClearColor(Color::kBlack),
		  mViewportWidth(0),
		  mViewportHeight(0),
          mLastTexture(nullptr),
          mLastShaderProgram(nullptr),
          mLastAlphaBlendState(false)
{
}

void Renderer::Init(const size_t viewportWidth, const size_t viewportHeigth)
{
	mViewportWidth = viewportWidth;
	mViewportHeight = viewportHeigth;

	mProjection = Math::Matrix4f::Ortho(0.0f, static_cast<const float>(viewportWidth),
										static_cast<const float>(viewportHeigth), 0.0f, -1.0f, 1.0f);

	glViewport(0, 0, static_cast<const int>(viewportWidth), static_cast<const int>(viewportHeigth));
	PACMAN_CHECK_GL_ERROR();
}

void Renderer::DrawFrame()
{
	glClearColor(mClearColor.GetRedFloat(), mClearColor.GetGreenFloat(),
				 mClearColor.GetBlueFloat(), mClearColor.GetAlphaFloat());
	PACMAN_CHECK_GL_ERROR();

	glClear(GL_COLOR_BUFFER_BIT);
	PACMAN_CHECK_GL_ERROR();

	const SceneManager& sceneManager = GetEngine().GetSceneManager();
	for (const std::shared_ptr<SceneNode>& node : sceneManager)
	{
        const IDrawable& drawable = *(node->GetDrawable());
		RenderDrawable(drawable, node->GetModelMatrix());
	}
}

void Renderer::RenderDrawable(const IDrawable& drawable, const Math::Matrix4f modelMatrix)
{
	const std::shared_ptr<VertexBuffer> vertexBuffer = drawable.GetVertexBuffer();
	const std::shared_ptr<ShaderProgram> shaderProgram = drawable.GetShaderProgram();
	const std::weak_ptr<Texture2D> texturePtr = drawable.GetTexture();
	const bool hasAlphaBlend = drawable.HasAlphaBlend();

	if (hasAlphaBlend && !mLastAlphaBlendState)
	{
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        PACMAN_CHECK_GL_ERROR();
        mLastAlphaBlendState = true;
	}
    else if (!hasAlphaBlend)
    {
        glDisable(GL_BLEND);
        PACMAN_CHECK_GL_ERROR();
        mLastAlphaBlendState = false;
    }

	if (const std::shared_ptr<Texture2D> texture = texturePtr.lock())
    {
        if (mLastTexture != texture.get())
        {
		    texture->Bind();
            mLastTexture = texture.get();
        }
    }

    if (shaderProgram.get() != mLastShaderProgram)
    {
	    shaderProgram->Bind();
        mLastShaderProgram = shaderProgram.get();
    }

	Math::Matrix4f modelProjection = (mProjection * modelMatrix).Transpose();
	shaderProgram->SetUniform(kModelProjMatrixUniformName, modelProjection);

	vertexBuffer->Bind();
	vertexBuffer->Draw();
	vertexBuffer->Unbind();
}

} // Pacman namespace
