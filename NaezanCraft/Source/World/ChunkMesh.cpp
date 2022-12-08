#include "../pch.h"
#include "ChunkMesh.h"
#include "Chunk.h"
#include "World.h"
#include "../Application.h"

#include "../Renderer/VertexArray.h"
#include "../Renderer/Buffer.h"

const std::array<glm::vec3, 4> ChunkMesh::vertices[]
{
	{ glm::vec3(0.5f, 0.5f, -0.5f),		glm::vec3(-0.5f, 0.5f, -0.5f),	glm::vec3(-0.5f, 0.5f, 0.5f),	glm::vec3(0.5f, 0.5f, 0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(0.5f, -0.5f, -0.5f) },
	{ glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.5f,  0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f,  0.5f) },
	{ glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f,  0.5f, -0.5f),	glm::vec3(0.5f,  0.5f, -0.5f) },
	{ glm::vec3(0.5f, -0.5f,  0.5f),	glm::vec3(0.5f, -0.5f, -0.5f),	glm::vec3(0.5f,  0.5f, -0.5f),	glm::vec3(0.5f,  0.5f,  0.5f) },
	{ glm::vec3(-0.5f, -0.5f, -0.5f),	glm::vec3(-0.5f, -0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f,  0.5f),	glm::vec3(-0.5f,  0.5f, -0.5f) }
};

const std::array<glm::u8vec3, 2> ChunkMesh::indices
{
	glm::u8vec3(0, 1, 2) , glm::u8vec3(2, 3, 0)
};

ChunkMesh::ChunkMesh(std::shared_ptr<Chunk>& chunk)
{
	parentChunk = chunk;
	//TO DO too much
	meshVertices.reserve(CHUNK_SIZE * 6);//�Ѹ��4������ * chunkSize 1024 -> 2304 576 (256 + 320)
	meshIndices.resize(CHUNK_SIZE * 6);//1536 -> 3000?

	CreateVertexBuffer();

	for (int i = 0; i < CHUNK_SIZE; ++i)
	{
		meshIndices[i * 6 + 0] = indices[0].x + 4 * i;
		meshIndices[i * 6 + 1] = indices[0].y + 4 * i;
		meshIndices[i * 6 + 2] = indices[0].z + 4 * i;
		meshIndices[i * 6 + 3] = indices[1].x + 4 * i;
		meshIndices[i * 6 + 4] = indices[1].y + 4 * i;
		meshIndices[i * 6 + 5] = indices[1].z + 4 * i;
	}

	vertexArray->Bind();
	CreateIndexBuffer();
}

ChunkMesh::~ChunkMesh() = default;

void ChunkMesh::CreateMesh()
{
	meshVertices.clear();//size to zero
	//GetSideChunk
	if (!GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x - 1), static_cast<int>(parentChunk->position.z)), LeftChunk))
	{
		LeftChunk = nullptr;
	}
	if (!GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x + 1), static_cast<int>(parentChunk->position.z)), RightChunk))
	{
		RightChunk = nullptr;
	}
	if (!GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x), static_cast<int>(parentChunk->position.z - 1)), BackChunk))
	{
		BackChunk = nullptr;
	}
	if (!GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(parentChunk->position.x), static_cast<int>(parentChunk->position.z + 1)), FrontChunk))
	{
		FrontChunk = nullptr;
	}

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int y = 0; y < CHUNK_Y; ++y)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				Block& block = parentChunk->chunkBlocks[x][y][z];
				if (block.blockType == BlockType::Air)
					continue;

				//GetBlockTypeAndGetTexCoord
				auto texcord = GetTexCoord(block.blockType);
				AddFaces(glm::vec3(x, y, z), block.blockType, texcord);
			}
		}
	}

	//SetVertexBuffer && texCoord
	if (!meshVertices.empty())
	{
		vertexBuffer->SetBufferData(meshVertices.size() * sizeof(VertTexCoord), &meshVertices.front());
	}
}

void ChunkMesh::AddFaces(const glm::vec3& pos, BlockType& type, const glm::vec2& texcoord)
{
	//X Left
	if (pos.x > 0)
	{
		//���� �����ڽ��� ���ٸ� ��������� �ȵǹǷ� ���� ���ʸ��� ������ �߰����ش�
		if (parentChunk->GetBlock(glm::vec3(pos.x - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left, texcoord);
	}
	else
	{
		//���� ���� ûũ�� �������� ���ٸ� 0��°��ġ ���ʸ��� ������ �߰����ش�
		if (LeftChunk == nullptr || LeftChunk->GetBlock(glm::vec3(CHUNK_X - 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Left, texcoord);
	}

	//X Right
	if (pos.x < CHUNK_X - 1)
	{
		//���� �����ڽ��� ���ٸ� ��������� �ȵǹǷ� ���� �����ʸ��� ������ �߰����ش�
		if (parentChunk->GetBlock(glm::vec3(pos.x + 1, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right, texcoord);
	}
	else
	{
		//���� ���� ûũ�� 0��°�� ���ٸ� CHUNK_X - 1��ġ �����ʸ��� ������ �߰����ش�
		if (RightChunk == nullptr || RightChunk->GetBlock(glm::vec3(0, pos.y, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Right, texcoord);
	}

	//Y Bottom
	if (pos.y > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y - 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Bottom, texcoord);
	}
	else
	{
		//Just add Bottom Face Once
		AddFace(pos, type, FaceType::Bottom, texcoord);
	}

	//Y Top
	if (pos.y < CHUNK_Y - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y + 1, pos.z)).IsTransparent())
			AddFace(pos, type, FaceType::Top, texcoord);
	}
	else
	{
		//Just add Top Face Once
		AddFace(pos, type, FaceType::Top, texcoord);
	}

	//Z Back
	if (pos.z > 0)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back, texcoord);
	}
	else
	{
		if (BackChunk == nullptr || BackChunk->GetBlock(glm::vec3(pos.x, pos.y, CHUNK_Z - 1)).IsTransparent())
			AddFace(pos, type, FaceType::Back, texcoord);
	}

	//Z Front
	if (pos.z < CHUNK_Z - 1)
	{
		if (parentChunk->GetBlock(glm::vec3(pos.x, pos.y, pos.z + 1)).IsTransparent())
			AddFace(pos, type, FaceType::Front, texcoord);
	}
	else
	{
		if (FrontChunk == nullptr || FrontChunk->GetBlock(glm::vec3(pos.x, pos.y, 0)).IsTransparent())
			AddFace(pos, type, FaceType::Front, texcoord);
	}
}

void ChunkMesh::AddFace(const glm::vec3& pos, const BlockType& Blocktype, const FaceType& faceType, const glm::vec2& texcoord)
{
	const std::array<glm::vec2, 4> texcoords {
		glm::vec2((SPRITE_WIDTH * texcoord.x) / ATLAS_WIDTH, (SPRITE_HEIGHT * (texcoord.y + 1.f)) / ATLAS_HEIGHT),
		glm::vec2((SPRITE_WIDTH * (texcoord.x + 1.f)) / ATLAS_WIDTH, (SPRITE_HEIGHT * (texcoord.y + 1.f)) / ATLAS_HEIGHT),
		glm::vec2((SPRITE_WIDTH * (texcoord.x + 1.f)) / ATLAS_WIDTH, (SPRITE_HEIGHT * texcoord.y) / ATLAS_HEIGHT),
		glm::vec2((SPRITE_WIDTH * texcoord.x) / ATLAS_WIDTH, (SPRITE_HEIGHT * texcoord.y) / ATLAS_HEIGHT)
	};

	switch (faceType)
	{
	case Top:
		meshVertices.push_back({ pos + vertices[Top][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Top][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Top][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Top][3],texcoords[3] });
		break;
	case Bottom:
		meshVertices.push_back({ pos + vertices[Bottom][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Bottom][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Bottom][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Bottom][3],texcoords[3] });
		break;
	case Front:
		meshVertices.push_back({ pos + vertices[Front][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Front][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Front][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Front][3],texcoords[3] });
		break;
	case Back:
		meshVertices.push_back({ pos + vertices[Back][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Back][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Back][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Back][3],texcoords[3] });
		break;
	case Right:
		meshVertices.push_back({ pos + vertices[Right][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Right][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Right][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Right][3],texcoords[3] });
		break;
	case Left:
		meshVertices.push_back({ pos + vertices[Left][0],texcoords[0] });
		meshVertices.push_back({ pos + vertices[Left][1],texcoords[1] });
		meshVertices.push_back({ pos + vertices[Left][2],texcoords[2] });
		meshVertices.push_back({ pos + vertices[Left][3],texcoords[3] });
		break;
	}
}

glm::vec2& ChunkMesh::GetTexCoord(BlockType& type)
{
	// TODO: insert return statement here
	auto temp = glm::vec2(13, 2);
	return temp;
}