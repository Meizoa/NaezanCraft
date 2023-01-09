#pragma once
#include "Mesh.h"

#include "Block.h"

enum FaceType
{
	//+y,-y,+z,-z,+x,-x
	Top, Bottom, Front, Back, Right, Left, FaceEnd
};

class Chunk;
class Water;
class VertexArray;
class VertexBuffer;
class IndexBuffer;

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::shared_ptr<Chunk>&& chunk);
	~ChunkMesh();

	void CreateMesh();
	void CreateBuffer();
	void RebuildBuffer(std::vector<VertTexCoord>& _rebuildVertices);
	void AddFaces(const glm::i8vec3& pos, Block& block);
	void AddFace(const glm::i8vec3& pos, Block& block, const FaceType& faceType, std::vector<VertTexCoord>& _meshVertices);
	void DeleteChunkMesh();

	glm::u16vec2 GetTexCoord(const BlockType& type);

	bool IsValidWaterMesh();

private:
	static const std::array<glm::i8vec3, 4> vertices[];
	//ûũ �ϳ��� ��� ���� ���⿡ ����ȴ�. 65,536 ���� ū ���� ������ ����ȴ�.
	std::vector<VertTexCoord> meshVertices;

	std::weak_ptr<Chunk> parentChunk;

	std::vector<VertTexCoord> waterVertices;
};