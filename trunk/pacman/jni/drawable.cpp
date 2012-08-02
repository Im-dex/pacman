#include "drawable.h"

#include "vertex_buffer.h"
#include "texture.h"
#include "shader_program.h"

namespace Pacman {

Drawable::Drawable(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<Texture2D> texture, std::shared_ptr<ShaderProgram> shaderProgram)
		: mVertexBuffer(vertexBuffer),
		  mTexture(texture),
		  mShaderProgram(shaderProgram)
{
}

} // Pacman namespace
