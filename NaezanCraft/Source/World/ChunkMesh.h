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
	glm::i8vec3 pos;
	glm::vec2 texcoord;
};

class ChunkMesh : public Mesh
{
public:
	ChunkMesh(std::shared_ptr<Chunk>& chunk);
	~ChunkMesh();

	void CreateMesh();
	void AddFaces(const glm::vec3& pos, BlockType& type, const glm::vec2& texcoord);
	void AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::vec2& texcoord);

	glm::vec2& GetTexCoord(BlockType& type);

private:
	static const std::array<glm::vec3, 4> vertices[];
	static const std::array<glm::u8vec3, 2> indices;
	//ûũ �ϳ��� ��� ���� ���⿡ ����ȴ�. 65,536 ���� ū ���� ������ ����ȴ�.
	std::vector<VertTexCoord> meshVertices;

	std::shared_ptr<Chunk> parentChunk;
	//Neighbor chunk
	std::shared_ptr<Chunk> LeftChunk;
	std::shared_ptr<Chunk> RightChunk;
	std::shared_ptr<Chunk> FrontChunk;
	std::shared_ptr<Chunk> BackChunk;
};