#include "../pch.h"
#include "Chunk.h"
#include "ChunkMesh.h"
#include "../Application.h"
#include "../Renderer/Renderer.h"
#include "../World/Generator/WorldGenerator.h"

const std::array<glm::vec3, 6> Chunk::nearFaces
{
	glm::vec3(-1, 0, 0),
	glm::vec3(1 , 0, 0),
	glm::vec3(0 , 0,-1),
	glm::vec3(0 , 0, 1),
	glm::vec3(0 ,-1, 0),
	glm::vec3(0 , 1, 0)
};

Chunk::Chunk(const glm::vec3& pos) :
	position(pos), chunkLoadState(ChunkLoadState::UnGenerated),
	chunkBox(glm::vec3(pos.x* CHUNK_X, pos.y* CHUNK_Y, pos.z* CHUNK_Z), CHUNK_X, CHUNK_Y, CHUNK_Z),
	LightMap(CHUNK_X, std::vector<std::vector<unsigned int>>(CHUNK_Y, std::vector<unsigned int>(CHUNK_Z)))
{
	//TO DO change blocktype
	//memset(&chunkBlocks[0][0][0], BlockType::Diamond, CHUNK_X * CHUNK_Y * CHUNK_Z * sizeof(Block));
	std::fill(&chunkBlocks[0][0][0], &chunkBlocks[0][0][0] + CHUNK_SIZE, BlockType::Air);
}

Chunk::~Chunk()
{
	LeftChunk.reset();
	RightChunk.reset();
	FrontChunk.reset();
	BackChunk.reset();
}

void Chunk::SetBlock(const glm::vec3& blockPos, BlockType type)
{
	chunkBlocks[blockPos.x][blockPos.y][blockPos.z].blockType = type;
}

void Chunk::SetBlock(int x, int y, int z, BlockType type)
{
	chunkBlocks[x][y][z].blockType = type;
}

Block& Chunk::GetBlock(const glm::vec3& blockPos)
{
	return chunkBlocks[blockPos.x][blockPos.y][blockPos.z];
}

Block& Chunk::GetBlock(int x, int y, int z)
{
	return chunkBlocks[x][y][z];
}

Block& Chunk::GetWorldBlock(const glm::vec3& blockPos)
{
	// TODO: insert return statement here
}

Block& Chunk::GetWorldBlock(int x, int y, int z)
{
	// �������̸� �ٸ������� ûũ����� ã�´� ���࿡ �װ͵� ���ٸ� ����Ʈ ������ ����
}

int Chunk::GetWorldLightLevel(int x, int y, int z)
{
	// �������̸� 15�� ���� �׿��� ��쿣 Ư�� ûũ ����� ����Ʈ���� ���� ������
	return 0;
}

void Chunk::SetWorldLightLevel(int x, int y, int z, int level)
{
}

void Chunk::SetupChunkNeighbor()
{
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x - 1), static_cast<int>(position.z)), LeftChunk))
	{
		LeftChunk.lock()->RightChunk = shared_from_this();
	}
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x + 1), static_cast<int>(position.z)), RightChunk))
	{
		RightChunk.lock()->LeftChunk = shared_from_this();
	}
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z - 1)), BackChunk))
	{
		BackChunk.lock()->FrontChunk = shared_from_this();
	}
	if (GET_World()->GetChunkByPos(std::pair<int, int>(static_cast<int>(position.x), static_cast<int>(position.z + 1)), FrontChunk))
	{
		FrontChunk.lock()->BackChunk = shared_from_this();
	}
}

void Chunk::CreateChunkMesh()
{
	if (chunkLoadState == ChunkLoadState::UnGenerated)
		chunkLoadState = ChunkLoadState::Generated;

	chunkMesh = std::make_unique<ChunkMesh>(shared_from_this());
	chunkMesh->CreateMesh();
}

void Chunk::GenerateTerrain(std::unique_ptr<WorldGenerator>& worldGenerator)
{
	worldGenerator->GenerateTerrain(shared_from_this());
}

void Chunk::CreateLightMap()
{
	std::queue<glm::ivec3> blockPosQue;
	//������ ������ ��� 15�� �����
	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			glm::ivec3 pos(x, CHUNK_Y - 1, z);
			blockPosQue.push(pos);
			LightMap[x][CHUNK_Y - 1][z] = 15;
		}
	}

	//�������� �Ʒ��κе��� �˻��ϸ鼭 ǥ��κ��� �������� ����
	while (!blockPosQue.empty())
	{
		glm::ivec3 localPos = blockPosQue.front();
		unsigned int lightLevel = LightMap[localPos.x][localPos.y][localPos.z];
		glm::ivec3 worldPos = 
		{
			localPos.x + position.x,
			localPos.y + position.y, 
			localPos.z + position.z
		};

		blockPosQue.pop();

		//TODO setting world chunk lighting
		for (int i = 0; i < 6; ++i)
		{
			int dx = worldPos.x + nearFaces[i].x;
			int dy = worldPos.y + nearFaces[i].y;
			int dz = worldPos.z + nearFaces[i].z;
			if (dy < 0) continue;

			Block& block = GetWorldBlock(dx, dy, dz);
			if (block.IsTransparent())
			{
				int maxHeight = GetBlockMaxHeight(localPos.x, localPos.z);
				//���� �¿� ���Ʒ��� ������ ���
				if (nearFaces[i].y == 0 || dy < maxHeight)
				{
					//������ �����ú��� �Ѵܰ� ���� �� ����
					if (GetWorldLightLevel(dx, dy, dz) < lightLevel - 1)
					{
						SetWorldLightLevel(dx, dy, dz, lightLevel - 1);
						blockPosQue.emplace(dx - position.x, dy, dz - position.z);
					}
				}
				//�ִ���� �̻��� ����
				else
				{
					//������ �ȵǾ� �ִٸ� �����ؼ� 15�� ����
					if (GetWorldLightLevel(dx, dy, dz) < lightLevel)
					{
						SetWorldLightLevel(dx, dy, dz, lightLevel);
						blockPosQue.emplace(dx - position.x, dy, dz - position.z);
					}
				}
			}
		}
	}
}

int Chunk::GetBlockMaxHeight(int x, int z)
{
	for (int y = CHUNK_Y - 1; y >= 0; --y) {
		if (!GetBlock(x, y, z).IsTransparent()) return y;
	}
	return 0;
}
