#include "../pch.h"
#include "Mesh.h"
#include "Chunk.h"
#include "Block.h"

const std::array<glm::vec3, 4> Mesh::vertices[]
{
	{ glm::vec3(0.5f, 0.5f, -0.5f) ,glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-0.5f, 0.5f, 0.5f),glm::vec3(0.5f, 0.5f, 0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f, -0.5f, -0.5f),glm::vec3(0.5f, -0.5f, -0.5f) },
	{ glm::vec3(-0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.5f,  0.5f,  0.5f),glm::vec3(-0.5f,  0.5f,  0.5f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f) ,glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-0.5f,  0.5f, -0.5f),glm::vec3(0.5f,  0.5f, -0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f) ,glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.5f,  0.5f, -0.5f),glm::vec3(0.5f,  0.5f,  0.5f) },
	{ glm::vec3(-0.5f, -0.5f, -0.5f) ,glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-0.5f,  0.5f,  0.5f),glm::vec3(-0.5f,  0.5f, -0.5f) }
};

const std::array<glm::u8vec3, 2> Mesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 1)
};

const std::array<glm::vec2, 4> Mesh::texcoords
{
	glm::vec2(0.0f, 1.0f) ,glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f)
};

Mesh::Mesh(std::shared_ptr<Chunk> chunk)
{
	parentChunk = chunk;
}

void Mesh::CreateMesh()
{
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = parentChunk->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				glm::vec3 tempPos = glm::vec3(x, y, z);
				glm::vec3 worldPosition;
				worldPosition.x = parentChunk->position.x * CHUNK_X + x;
				worldPosition.y = y;
				worldPosition.x = parentChunk->position.z * CHUNK_Z + z;
				worldPosition /= 2;
				AddFaces(tempPos, block.blockType);
			}
		}
	}
}

void Mesh::AddFaces(glm::vec3& pos, BlockType& type)
{
	//X Left
	if (pos.x > 0)
	{
		//���� �����ڽ��� ���ٸ� ��������� �ȵǹǷ� ���� ���ʸ��� ������ �߰����ش�
		if (parentChunk->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left);
	}
	else
	{
		//���� ���� ûũ�� �������� ���ٸ� 0��°��ġ ���ʸ��� ������ �߰����ش�
		AddFace(pos, type, FaceType::Left);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//���� �����ڽ��� ���ٸ� ��������� �ȵǹǷ� ���� �����ʸ��� ������ �߰����ش�
		if (parentChunk->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right);
	}
	else
	{
		//���� ���� ûũ�� 0��°�� ���ٸ� CHUNK_X - 1��ġ �����ʸ��� ������ �߰����ش�
		AddFace(pos, type, FaceType::Right);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Bottom);
	}
	else
	{
		AddFace(pos, type, FaceType::Bottom);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Top);
	}
	else
	{
		AddFace(pos, type, FaceType::Top);
	}

	//Z Back
	if (pos.z > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}
	else
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		AddFace(pos, type, FaceType::Front);
	}
	else
	{
		AddFace(pos, type, FaceType::Front);
	}
}

void Mesh::AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType)
{

}
