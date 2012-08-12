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

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	PACMAN_CHECK_GL_ERROR();

	int i = 0;
	for (auto node : mSceneManager->GetNodes())
	{
		if (i == 1)
		{
			glEnable(GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		RenderDrawable(node->GetDrawable(), node->GetModelMatrix());

		if (i == 1)
		{
			glDisable(GL_BLEND);
		}

		i++;
	}
}

void Renderer::RenderDrawable(const std::shared_ptr<IDrawable> drawable, const Math::Matrix4f modelMatrix)
{
	auto texture = drawable->GetTexture();
	auto vertexBuffer = drawable->GetVertexBuffer();
	auto shaderProgram = drawable->GetShaderProgram();

	if (texture != nullptr)
		texture->Bind();

	shaderProgram->Bind();

	Math::Matrix4f modelProjection = (mProjection * modelMatrix).Transpose();
	shaderProgram->SetUniform(kModelProjMatrixUniformName, modelProjection);
	vertexBuffer->Bind();
	vertexBuffer->Draw();
	vertexBuffer->Unbind();
}

} // Pacman namespace
