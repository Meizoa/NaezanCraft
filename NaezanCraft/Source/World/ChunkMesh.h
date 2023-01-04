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
	uint8_t lightlevel;
	uint8_t AO;
};

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::shared_ptr<Chunk>&& chunk);
	~ChunkMesh();

	void CreateMesh();
	void CreateBuffer();
	void AddFaces(const glm::u8vec3& pos, Block& block);
	void AddFace(const glm::u8vec3& pos, Block& block, const FaceType& faceType);
	void DeleteChunkMesh();

	glm::u16vec2 GetTexCoord(const BlockType& type);

private:
	static const std::array<glm::u8vec3, 4> vertices[];
	static const std::array<glm::u8vec3, 2> indices;
	//ûũ �ϳ��� ��� ���� ���⿡ ����ȴ�. 65,536 ���� ū ���� ������ ����ȴ�.
	std::vector<VertTexCoord> meshVertices;

	std::weak_ptr<Chunk> parentChunk;
};