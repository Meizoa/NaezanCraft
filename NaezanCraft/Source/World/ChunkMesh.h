#pragma once
#include "Mesh.h"

#include "Block.h"

enum FaceType
{
	//+y,-y,+z,-z,+x,-x
	Top, Bottom, Front, Back, Right, Left, FaceEnd
};

class Chunk;
class VertexArray;
class VertexBuffer;
class IndexBuffer;

struct VertTexCoord
{
	glm::u8vec3 pos;
	glm::u16vec2 texcoord;
	uint8_t facelight;
};

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::shared_ptr<Chunk>&& chunk);
	~ChunkMesh();

	void CreateMesh();
	void AddFaces(const glm::u8vec3& pos, BlockType& type, const glm::u16vec2& texcoord);
	void AddFace(const glm::u8vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::u16vec2& texcoord);

	glm::u16vec2 GetTexCoord(BlockType& type);

	bool IsEmptyChunk(std::weak_ptr<Chunk> const& chunk);

private:
	static const std::array<glm::u8vec3, 4> vertices[];
	static const std::array<glm::u8vec3, 2> indices;
	static const std::array<uint8_t, 6> faceslight;
	//ûũ �ϳ��� ��� ���� ���⿡ ����ȴ�. 65,536 ���� ū ���� ������ ����ȴ�.
	std::vector<VertTexCoord> meshVertices;

	std::weak_ptr<Chunk> parentChunk;
};