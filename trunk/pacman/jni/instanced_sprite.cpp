#include "instanced_sprite.h"

#include <cassert>

#include "base.h"
#include "error.h"
#include "texture.h"
#include "vertex_buffer.h"
#include "shader_program.h"

namespace Pacman {

static const size_t kSpriteVertexCount = 4;
static const size_t kSpriteIndexCount = 6;
static const Color kDefaultColor = Color::kGreen;
static const TextureRegion kDefaultRegion = TextureRegion(Math::Vector2f::kZero, 1.0f, 1.0f);

template <typename VertexT>
static FORCEINLINE void FillSpriteVertex(VertexT& vertex, const VertexT& baseVertex, const SpritePosition& position)
{
    vertex = baseVertex;
    vertex.x += position.GetX();
    vertex.y += position.GetY();
}

template <typename VertexT>
static void FillVertexData(const SpriteRegion& region, const std::vector<SpritePosition>& positions,
                           std::vector<VertexT>& vertices, std::vector<uint16_t>& indices)
{
    //                  base sprite
    //                           	____
    // 				 |\          	\ 	|  
    // 	 	 	 |	 | \    ^    | 	 \ 	|  ^
    // 	 	 	 v	 |  \   | +  v	  \	|  |
    // 	 	 	 	 |___\        	   \|
    //				   ->             ->

    static const std::array<uint16_t, kSpriteIndexCount> kBaseIndices = { 0, 1, 2, 0, 2, 3 };
    std::array<VertexT, kSpriteVertexCount> baseVertices;

    // fiil base position data
    baseVertices[0].x = region.GetPosX();
    baseVertices[0].y = region.GetPosY();

    baseVertices[1].x = region.GetLeftBottomPosX();
    baseVertices[1].y = region.GetLeftBottomPosY();

    baseVertices[2].x = region.GetRightBottomPosX();
    baseVertices[2].y = region.GetRightBottomPosY();

    baseVertices[3].x = region.GetRightTopPosX();
    baseVertices[3].y = region.GetRightTopPosY();

    // resize vectors to data size
    vertices.resize(kSpriteVertexCount * positions.size());
    indices.resize(kSpriteIndexCount * positions.size());

    // fill position data
    for (size_t i = 0; i < positions.size(); i++)
    {
        const SpritePosition& pos = positions[i];
        for (size_t j = 0; j < kSpriteVertexCount; j++) // compiler will must apply loop unroll here
        {
            VertexT& vertex = vertices[i*kSpriteVertexCount + j];
            FillSpriteVertex(vertex, baseVertices[j], pos);
        }
    }

    // fill index data
    for (size_t i = 0; i < positions.size(); i++)
    {
        for (size_t j = 0; j < kSpriteIndexCount; j++)
        {
            indices[i*kSpriteIndexCount + j] = kBaseIndices[j] + i*kSpriteVertexCount;
        }
    }
}

static FORCEINLINE void FillColor(ColorVertex& vertex, const Color& color)
{
    vertex.r = color.GetRedFloat();
    vertex.g = color.GetGreenFloat();
    vertex.b = color.GetBlueFloat();
    vertex.a = color.GetAlphaFloat();
}

static FORCEINLINE void FillTexCoord(TextureVertex& vertex, const Math::Vector2f texCoord)
{
    vertex.u = texCoord.GetX();
    vertex.v = texCoord.GetY();
}

//===========================================================================================================

InstancedSprite::InstancedSprite(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, const Color& leftBottom,
			                     const Color& rightBottom, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                                 const std::vector<SpritePosition>& instances, const bool instanceHideEnabled)
	           : mShaderProgram(shaderProgram),
	             mTexture(nullptr),
		         mVertexBuffer(nullptr),
		         mAlphaBlend(alphaBlend),
                 mInstanceHideEnabled(instanceHideEnabled),
                 mInstancesCount(instances.size())
{
	InitByColor(region, leftTop, rightTop, leftBottom, rightBottom, instances);
}

InstancedSprite::InstancedSprite(const SpriteRegion& region, std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend,
                                 const std::vector<SpritePosition>& instances, const bool instanceHideEnabled)
	           : mShaderProgram(shaderProgram),
	             mTexture(nullptr),
		         mVertexBuffer(nullptr),
		         mAlphaBlend(alphaBlend),
                 mInstanceHideEnabled(instanceHideEnabled),
                 mInstancesCount(instances.size())
{
	InitByColor(region, kDefaultColor, kDefaultColor, kDefaultColor, kDefaultColor, instances);
}

InstancedSprite::InstancedSprite(const SpriteRegion& region, const TextureRegion& textureRegion, std::shared_ptr<Texture2D> texture,
			                     std::shared_ptr<ShaderProgram> shaderProgram, const bool alphaBlend, const std::vector<SpritePosition>& instances,
                                 const bool instanceHideEnabled)
	           : mShaderProgram(shaderProgram),
	             mTexture(texture),
		         mVertexBuffer(nullptr),
		         mAlphaBlend(alphaBlend),
                 mInstanceHideEnabled(instanceHideEnabled),
                 mInstancesCount(instances.size())
{
	InitByTexture(region, textureRegion, instances);
}

InstancedSprite::InstancedSprite(const SpriteRegion& region, std::shared_ptr<Texture2D> texture, std::shared_ptr<ShaderProgram> shaderProgram,
                                 const bool alphaBlend, const std::vector<SpritePosition>& instances, const bool instanceHideEnabled)
	           : mShaderProgram(shaderProgram),
	             mTexture(texture),
		         mVertexBuffer(nullptr),
		         mAlphaBlend(alphaBlend),
                 mInstanceHideEnabled(instanceHideEnabled),
                 mInstancesCount(instances.size())
{
	InitByTexture(region, kDefaultRegion, instances);
}

void InstancedSprite::HideInstance(const size_t index)
{
    PACMAN_CHECK_ERROR(index < mInstancesCount, ErrorCode::BadArgument);
    std::vector<uint16_t>& indexData = mVertexBuffer->LockIndexData();
    const size_t instanceOffset = index * kSpriteIndexCount;
    indexData.erase(indexData.begin() + instanceOffset, indexData.begin() + instanceOffset + kSpriteIndexCount);
    mVertexBuffer->UnlockIndexData();
}

VertexBuffer& InstancedSprite::GetVertexBuffer() const
{
	return *mVertexBuffer;
}

std::weak_ptr<Texture2D> InstancedSprite::GetTexture() const
{
	return mTexture;
}

ShaderProgram& InstancedSprite::GetShaderProgram() const
{
	return *mShaderProgram;
}

bool InstancedSprite::HasAlphaBlend() const
{
	return mAlphaBlend;
}

//TODO: remove copy paste!!!!!!!!!!!!!!!!!!!
void InstancedSprite::InitByColor(const SpriteRegion& region, const Color& leftTop, const Color& rightTop, 
					         	  const Color& leftBottom, const Color& rightBottom, const std::vector<SpritePosition>& instances)
{
    PACMAN_CHECK_ERROR(instances.size() > 0, ErrorCode::BadArgument);

    std::vector<ColorVertex> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(kSpriteVertexCount * instances.size());
    indices.reserve(kSpriteIndexCount * instances.size());
    FillVertexData(region, instances, vertices, indices);

    assert(vertices.size() / instances.size() == kSpriteVertexCount);
    assert(indices.size() / instances.size() == kSpriteIndexCount);

    for (size_t i = 0; i < instances.size(); i++)
    {
        FillColor(vertices[i*kSpriteVertexCount + 0], leftTop);
        FillColor(vertices[i*kSpriteVertexCount + 1], leftBottom);
        FillColor(vertices[i*kSpriteVertexCount + 2], rightBottom);
        FillColor(vertices[i*kSpriteVertexCount + 3], rightTop);
    }

    BufferUsage indexBufferUsage = mInstanceHideEnabled ? BufferUsage::Dynamic : BufferUsage::Static;
	mVertexBuffer = std::make_shared<VertexBuffer>(vertices, indices, BufferUsage::Static, indexBufferUsage);
}

void InstancedSprite::InitByTexture(const SpriteRegion& region, const TextureRegion& textureRegion, const std::vector<SpritePosition>& instances)
{
    PACMAN_CHECK_ERROR(instances.size() > 0, ErrorCode::BadArgument);

    std::vector<TextureVertex> vertices;
    std::vector<uint16_t> indices;

    vertices.reserve(kSpriteVertexCount * instances.size());
    indices.reserve(kSpriteIndexCount * instances.size());
    FillVertexData(region, instances, vertices, indices);

    assert(vertices.size() / instances.size() == kSpriteVertexCount);
    assert(indices.size() / instances.size() == kSpriteIndexCount);

	for (size_t i = 0; i < instances.size(); i++)
    {
        FillTexCoord(vertices[i*kSpriteVertexCount + 0], textureRegion.GetPosition());
        FillTexCoord(vertices[i*kSpriteVertexCount + 1], Math::Vector2f(textureRegion.GetPosX(), textureRegion.GetPosY() + textureRegion.GetHeight()));
        FillTexCoord(vertices[i*kSpriteVertexCount + 2], Math::Vector2f(textureRegion.GetPosX() + textureRegion.GetWidth(), textureRegion.GetPosY() + textureRegion.GetHeight()));
        FillTexCoord(vertices[i*kSpriteVertexCount + 3], Math::Vector2f(textureRegion.GetPosX() + textureRegion.GetWidth(), textureRegion.GetPosY()));
    }

    BufferUsage indexBufferUsage = mInstanceHideEnabled ? BufferUsage::Dynamic : BufferUsage::Static;
	mVertexBuffer = std::make_shared<VertexBuffer>(vertices, indices, BufferUsage::Static, indexBufferUsage);
}

} // Pacman namespace
