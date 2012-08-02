#include "renderer.h"
#include "error.h"
#include "scene_manager.h"
#include "scene_node.h"
#include "drawable.h"
#include "texture.h"
#include "shader_program.h"
#include "vertex_buffer.h"

#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>

namespace Pacman {

static const char* kProjectionUniformName = "mProjectionMatrix";
static const char* kModelMatrixUniformName = "mModelMatrix";

Renderer::Renderer()
		: mProjection(),
		  mClearColor(Color::kBlack),
		  mSceneManager()
{
}

void Renderer::Init(const size_t viewportWidth, const size_t viewportHeigth)
{
	mProjection = Math::Matrix4f::Ortho(0.0f, static_cast<const float>(viewportWidth), 0.0f,
										static_cast<const float>(viewportHeigth), -1.0f, 1.0f);

	glViewport(0, 0, static_cast<const int>(viewportWidth), static_cast<const int>(viewportHeigth));
	PACMAN_CHECK_GL_ERROR();
}

void Renderer::DrawFrame()
{
	glClearColor(mClearColor.GetRedFloat(), mClearColor.GetGreenFloat(),
				 mClearColor.GetBlueFloat(), mClearColor.GetAlphaFloat());
	PACMAN_CHECK_GL_ERROR();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	PACMAN_CHECK_GL_ERROR();

	for (auto node : mSceneManager->GetNodes())
	{
		RenderDrawable(node.GetDrawable(), node.GetModelMatrix());
	}
}

void Renderer::RenderDrawable(const Drawable& drawable, const Math::Matrix4f modelMatrix)
{
	auto texture = drawable.GetTexture();
	auto vertexBuffer = drawable.GetVertexBuffer();
	auto shaderProgram = drawable.GetShaderProgram();

	if (texture != nullptr)
		texture->Bind();

	shaderProgram->Bind();

	Math::Matrix4f projection(mProjection);
	Math::Matrix4f model(modelMatrix);
	projection.Transpose();
	model.Transpose();
	shaderProgram->SetUniform(kProjectionUniformName, projection);
	//shaderProgram->SetUniform(kModelMatrixUniformName, model);

	vertexBuffer->Bind(shaderProgram);

	vertexBuffer->Draw();
}

} // Pacman namespace
