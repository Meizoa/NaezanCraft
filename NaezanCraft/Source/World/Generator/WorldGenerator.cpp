#include "../../pch.h"
#include "WorldGenerator.h"
#include "../Chunk.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

WorldGenerator::WorldGenerator() : gen(rd())
{
	fastNoise.SetFrequency(.01f);
	fastNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	fastNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
	fastNoise.SetFractalOctaves(7);
}

WorldGenerator::~WorldGenerator() = default;

void WorldGenerator::GenerateTerrain(std::weak_ptr<Chunk> chunk)
{
	const glm::vec3& chunkPos = chunk.lock()->position;

	SetHeightMap(chunk);
	//��ü �� �ִ� ���� ã��
	int maxHeight = WATER_HEIGHT;
	for (int i = 0; i < CHUNK_X; ++i)
	{
		maxHeight = std::max(maxHeight, *std::max_element(heightMap[i].begin(), heightMap[i].end()));
	}
	//chunk.lock()->chunkBox.y = maxHeight;//SetBoxHeight
	SetChunkBlocks(maxHeight, chunk);
}

void WorldGenerator::SetHeightMap(std::weak_ptr<Chunk> chunk)
{
	//TODO��ġ�� �� �� Ȯ���ϰ� �ٸ��ɷ�
	auto pos = chunk.lock()->position;
	pos.x *= CHUNK_X;
	pos.z *= CHUNK_Z;

	for (int x = 0; x < CHUNK_X; ++x)
	{
		for (int z = 0; z < CHUNK_Z; ++z)
		{
			//GetBiome->GetNoiseArea();
			int h = GetBlockHeight(pos.x + static_cast<float>(x), pos.z + static_cast<float>(z));
			if (h > 127)
				h = 127;
			heightMap[x][z] = h;
		}
	}
}

int WorldGenerator::GetBlockHeight(float x, float z)
{
	float noise = (fastNoise.GetNoise(x, z) + 1.f) / 2.f;
	return static_cast<int>(noise * 35.f + 43.f);
}

void WorldGenerator::SetChunkBlocks(int maxHeight, std::weak_ptr<Chunk> chunk)
{
	std::vector<glm::vec3> treePositions;

	for (int y = 0; y <= maxHeight; ++y)
	{
		for (int x = 0; x < CHUNK_X; ++x)
		{
			for (int z = 0; z < CHUNK_Z; ++z)
			{
				//SetBlockType
				int h = heightMap[x][z];
				//���� ��� ��ġ���� ���ڸ�
				if (chunk.lock()->GetBlock(x, y, z).blockType == BlockType::None)
				{
					continue;
				}

				if (y > h)
				{
					if (y <= WATER_HEIGHT)
					{
						//Water
						chunk.lock()->SetBlock(x, y, z, BlockType::Water);
					}
				}
				//����
				else if (y == h)
				{
					if (y >= WATER_HEIGHT)
					{
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)
						//TODO GetBiome(x, z).GetSurfaceBlockType();
						chunk.lock()->SetBlock(x, y, z, BlockType::Grass);

						//TODO Flower, Tree, Catus, Grass

						std::uniform_int_distribution<int> dis(0, 70);
						if (dis(gen) == 0)
						{
							treePositions.emplace_back(x, y + 1, z);
						}
					}
					else
					{
						//RandomBaseMakeUnderWaterCluster(Mountain, Forest, Desert, Ocean) -> Circle?
						//Defualt set block by biome, stone(mountain) or dirt(forest) or sand(desert)
						//TODO GetBiome(x, z).GetInWaterBlockType();
						chunk.lock()->SetBlock(x, y, z, BlockType::Sand);
					}
				}
				else if (y >= h - 1)
				{
					if (y >= WATER_HEIGHT)
					{
						chunk.lock()->SetBlock(x, y, z, BlockType::Dirt);
					}
					else
					{
						chunk.lock()->SetBlock(x, y, z, BlockType::Sand);
					}
				}
				else if (y >= h - 2)
				{
					if (y >= WATER_HEIGHT)
					{
						std::uniform_int_distribution<int> dis(0, 3);
						if (dis(gen) < 3)
							chunk.lock()->SetBlock(x, y, z, BlockType::Dirt);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
					else
					{
						chunk.lock()->SetBlock(x, y, z, BlockType::Sand);
					}
				}
				//����ִ� ���� �Ʒ�3ĭ
				else if (y >= h - 3)
				{
					//set block by biome, stone(mountain) or dirt(forest) or sand(desert)
					//TODO GetBiome(x, z).GetUnderGroundBlockType();
					chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
				}
				//�ǹٴ�
				else if (y == 0)
				{
					chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
				}
				//�ǹٴ�3ĭ��
				else if (y <= 3)
				{
					//Stone or bedrock
					if (y == 1)
					{
						std::uniform_int_distribution<int> dis(0, 3);
						if (dis(gen) < 3)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
					else if (y == 2)
					{
						std::uniform_int_distribution<int> dis(0, 2);
						if (dis(gen) < 2)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
					else
					{
						std::uniform_int_distribution<int> dis(0, 1);
						if (dis(gen) < 1)
							chunk.lock()->SetBlock(x, y, z, BlockType::Bedrock);
						else
							chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
					}
				}
				//������
				else
				{
					//stone
					chunk.lock()->SetBlock(x, y, z, BlockType::Stone);
				}
			}
		}
	}

	//Generate Tree
	for (auto& treePos : treePositions)
	{
		//��ġ�����ۿ� ûũ�� ���ٸ�?

		if (chunk.lock()->GetBlock(treePos).blockType == BlockType::Air)
		{
			if (chunk.lock()->GetBlock(treePos.x, treePos.y - 1, treePos.z).IsGrowable())
			{
				GenerateTree(chunk, treePos.x, treePos.y, treePos.z);
			}
		}
	}
}

void WorldGenerator::GenerateTree(std::weak_ptr<Chunk> chunk, int x, int y, int z)
{
	struct TreeInfo
	{
		TreeInfo(glm::vec3 _pos, BlockType type)
			: pos(_pos), block(type)
		{}
		glm::vec3 pos;
		BlockType block;
	};

	std::vector<TreeInfo> treeBlocks;
	BlockType logType;
	BlockType leavesType;
	int height;
	int topHeight;

	std::uniform_int_distribution<int> disTreeType(0, 2);
	if (disTreeType(gen) < 2)
	{
		logType = BlockType::OakLog;
		leavesType = BlockType::OakLeaves;
	}
	else
	{
		logType = BlockType::BirchLog;
		leavesType = BlockType::BirchLeaves;
	}

	//���̴� 5~7 6�� 50%Ȯ�� 5�� 30%Ȯ�� 7�� 20%Ȯ���� ����
	std::uniform_int_distribution<int> disHeight(0, 10);
	int randomheight = disHeight(gen);
	if (randomheight < 5)
	{
		height = 6;
	}
	else if (randomheight < 8)
	{
		height = 5;
	}
	else
	{
		height = 7;
	}

	topHeight = height + y;

	//3X3
	for (int dx = x - 1; dx <= x + 1; ++dx)
	{
		for (int dz = z - 1; dz <= z + 1; ++dz)
		{
			for (int dy = topHeight; dy >= topHeight - 1; --dy)
			{
				if (dx == x && dz == z && dy != topHeight)
				{
					//dx, dz�� 0�̸� �ٱ�
					treeBlocks.emplace_back(glm::vec3(x, dy, z), logType);
				}
				else
				{
					//���κ���50%Ȯ���� ������ ����
					if ((dx == x - 1 || dx == x + 1) && (dz == z - 1 || dz == z + 1))
					{
						//���࿡ topHeight��� ���κ��� ����X
						if (dy == topHeight)
						{
							continue;
						}

						std::uniform_int_distribution<int> disHalf(0, 1);
						if (disHalf(gen) == 0)
						{
							treeBlocks.emplace_back(glm::vec3(dx, dy, dz), leavesType);
						}
					}
					else
					{
						treeBlocks.emplace_back(glm::vec3(dx, dy, dz), leavesType);
					}
				}
			}
		}
	}

	//5X5
	for (int dx = x - 2; dx <= x + 2; ++dx)
	{
		for (int dz = z - 2; dz <= z + 2; ++dz)
		{
			for (int dy = topHeight - 2; dy >= topHeight - 3; --dy)
			{
				if (dx == x && dz == z)
				{
					//dx, dz�� 0�̸� �ٱ�
					treeBlocks.emplace_back(glm::vec3(x, dy, z), logType);
				}
				else
				{
					//���κ���50%Ȯ���� ������ ����
					if ((dx == x - 2 || dx == x + 2) && (dz == z - 2 || dz == z + 2))
					{
						std::uniform_int_distribution<int> disHalf(0, 1);
						if (disHalf(gen) == 0)
						{
							treeBlocks.emplace_back(glm::vec3(dx, dy, dz), leavesType);
						}
					}
					else
					{
						treeBlocks.emplace_back(glm::vec3(dx, dy, dz), leavesType);
					}
				}
			}
		}
	}

	//y~topHeight - 4
	for (int dy = y; dy <= topHeight - 4; ++dy)
	{
		//x, z��ġ�� �ٱ⸸ ����
		treeBlocks.emplace_back(glm::vec3(x, dy, z), logType);
	}

	//GenerateFunction
	for (auto& blockInfo : treeBlocks)
	{
		chunk.lock()->SetBlock(blockInfo.pos, blockInfo.block);
	}
	treeBlocks.clear();
}
