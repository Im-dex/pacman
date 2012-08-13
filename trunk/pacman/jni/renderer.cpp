#include "renderer.h"
#include "error.h"
#include "scene_manager.h"
#include "scene_node.h"
#include "drawable.h"
#include "texture.h"
#include "shader_program.h"
#include "vertex_buffer.h"

#include "base.h"

#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

namespace Pacman {

static const char* kProjectionUniformName = "mProjectionMatrix";
static const char* kModelMatrixUniformName = "mModelMatrix";
static const char* kModelProjMatrixUniformName = "mModelProjectionMatrix";

Renderer::Renderer()
		: mProjection(),
		  mClearColor(Color::kBlack),
		  mSceneManager()
{
}

void Renderer::Init(const size_t viewportWidth, const size_t viewportHeigth)
{
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

	for (auto node : mSceneManager->GetNodes())
	{
		RenderDrawable(node->GetDrawable(), node->GetModelMatrix());
	}
}

void Renderer::RenderDrawable(const std::shared_ptr<IDrawable> drawable, const Math::Matrix4f modelMatrix)
{
	auto texture = drawable->GetTexture();
	auto shaderProgram = drawable->GetShaderProgram();
	bool hasAlphaBlend = drawable->HasAlphaBlend();

	if (hasAlphaBlend)
	{
		glEnable(GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if (texture != nullptr)
		texture->Bind();
	shaderProgram->Bind();

	Math::Matrix4f modelProjection = (mProjection * modelMatrix).Transpose();
	shaderProgram->SetUniform(kModelProjMatrixUniformName, modelProjection);

	auto vertexBuffer = drawable->GetVertexBuffer();
	vertexBuffer->Bind();
	vertexBuffer->Draw();
	vertexBuffer->Unbind();

	shaderProgram->Unbind();
	if (texture != nullptr)
		texture->Unbind();

	if (hasAlphaBlend)
	{
		glDisable(GL_BLEND);
	}
}

} // Pacman namespace
