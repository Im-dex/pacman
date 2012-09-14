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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	PACMAN_CHECK_GL_ERROR();
}

void Renderer::DrawFrame()
{
	glClearColor(mClearColor.GetRedFloat(), mClearColor.GetGreenFloat(),
				 mClearColor.GetBlueFloat(), mClearColor.GetAlphaFloat());
	PACMAN_CHECK_GL_ERROR();

	glClear(GL_COLOR_BUFFER_BIT);
	PACMAN_CHECK_GL_ERROR();

	SceneManager& sceneManager = GetEngine()->GetSceneManager();
	for (auto node : sceneManager.GetNodes())
	{
		RenderDrawable(node->GetDrawable(), node->GetModelMatrix());
	}
}

void Renderer::RenderDrawable(const std::shared_ptr<IDrawable> drawable, const Math::Matrix4f modelMatrix)
{
    PACMAN_CHECK_ERROR(drawable != nullptr, ErrorCode::InvalidState);

	std::shared_ptr<VertexBuffer> vertexBuffer = drawable->GetVertexBuffer();
	std::shared_ptr<ShaderProgram> shaderProgram = drawable->GetShaderProgram();
	std::weak_ptr<Texture2D> texture = drawable->GetTexture();
	bool hasAlphaBlend = drawable->HasAlphaBlend();
    
    PACMAN_CHECK_ERROR((vertexBuffer != nullptr) && (shaderProgram != nullptr), ErrorCode::InvalidState);

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

	if (auto texturePtr = texture.lock())
    {
        //if (mLastTexture != texturePtr.get())
       // {
		    texturePtr->Bind();
            mLastTexture = texturePtr.get();
        //}
    }

    //if (shaderProgram.get() != mLastShaderProgram)
    //{
	    shaderProgram->Bind();
      //  mLastShaderProgram = shaderProgram.get();
   // }

	Math::Matrix4f modelProjection = (mProjection * modelMatrix).Transpose();
	shaderProgram->SetUniform(kModelProjMatrixUniformName, modelProjection);

	vertexBuffer->Bind();
	vertexBuffer->Draw();
	vertexBuffer->Unbind();
}

} // Pacman namespace
