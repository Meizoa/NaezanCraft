#include "../../pch.h"
#include "../../World/Block.h"
#include "../../World/Chunk.h"
#include "../Application.h"
#include "Ray.h"

Block Ray::BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir, glm::vec3& outPosition)
{
	std::weak_ptr<Chunk> curChunk;
	int dx = ray_start.x, dz = ray_start.z;
	if (ray_start.x < 0)
	{
		dx = std::floor(dx);
	}
	if (ray_start.y < 0)
	{
		dz = std::floor(dz);
	}
	std::pair<int, int> key(dx, dz);
	if (key.first < 0)
	{
		key.first -= (CHUNK_X - 1);
	}
	key.first /= CHUNK_X;

	if (key.second < 0)
	{
		key.second -= (CHUNK_Z - 1);
	}
	key.second /= CHUNK_Z;

	GET_World()->GetChunkByPos(key, curChunk);

	Block block(BlockType::Air);
	if (Chunk::IsEmptyChunk(curChunk))
		return block;

	glm::vec3 blockPos;
	//ûũ�� �� ������ġ(����ó��)
	blockPos.x = ray_start.x >= 0 ? static_cast<int>(ray_start.x) % CHUNK_X : ((static_cast<int>(ray_start.x) % CHUNK_X) + CHUNK_X) % CHUNK_X;
	blockPos.y = static_cast<int>(ray_start.y);
	blockPos.z = ray_start.z >= 0 ? static_cast<int>(ray_start.z) % CHUNK_Z : ((static_cast<int>(ray_start.z) % CHUNK_Z) + CHUNK_Z) % CHUNK_Z;

	float stepX = dir.x >= 0 ? 1 : -1;
	float stepY = dir.y >= 0 ? 1 : -1;
	float stepZ = dir.z >= 0 ? 1 : -1;

	//������ �˻��� �� ���� ��ġ
	/*float curIndexX = (blockPos.x + stepX) + curChunk.lock()->position.x * CHUNK_X;
	float curIndexY = (blockPos.y + stepY);
	float curIndexZ = (blockPos.z + stepZ) + curChunk.lock()->position.z * CHUNK_Z;*/
	float curIndexX = dir.x >= 0 ? 1 : 0;
	float curIndexY = dir.y >= 0 ? 1 : 0;
	float curIndexZ = dir.z >= 0 ? 1 : 0;

	//��������ġ - ������ ������ġ
	float tMaxX = ray_start.x >= 0 ? (std::floor(ray_start.x) + curIndexX - ray_start.x) / dir.x : (std::ceil(ray_start.x) + curIndexX - ray_start.x) / dir.x;
	float tMaxY = ray_start.y >= 0 ? (std::floor(ray_start.y) + curIndexY - ray_start.y) / dir.y : (std::ceil(ray_start.y) + curIndexY - ray_start.y) / dir.y;
	float tMaxZ = ray_start.z >= 0 ? (std::floor(ray_start.z) + curIndexZ - ray_start.z) / dir.z : (std::ceil(ray_start.z) + curIndexZ - ray_start.z) / dir.z;
	//��ĭ �����Ҷ��� �밢���� ���� �ܼ��� dir�� ������ �̷�����ٴ°� ����� �����غ��� �� �� �ִ�.
	float tDeltaX = stepX / dir.x;
	float tDeltaY = stepY / dir.y;
	float tDeltaZ = stepZ / dir.z;

	float fDistance = 0;
	//RayBlock ���࿡ ���������� ����ų� ���� ã���� �ݺ����� �����´�
	while (block.IsFluid() && fDistance < 10) {
		if (tMaxX < tMaxY) {
			//x�� ���� �۴ٴ� �� x���� ���� ��Ҵٴ� ��
			if (tMaxX < tMaxZ) {
				blockPos.x += stepX;
				fDistance = tMaxX;
				tMaxX += tDeltaX;
				if (blockPos.x >= CHUNK_X || blockPos.x < 0)
				{
					if (stepX > 0)
					{
						curChunk = curChunk.lock()->RightChunk;
						blockPos.x = 0;
					}
					else
					{
						curChunk = curChunk.lock()->LeftChunk;
						blockPos.x = 15;
					}
					if (Chunk::IsEmptyChunk(curChunk))
						return block;
				}
			}
			//z���� ��Ҵٴ� ��
			else {
				blockPos.z += stepZ;
				fDistance = tMaxZ;
				tMaxZ += tDeltaZ;
				if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
				{
					if (stepZ > 0)
					{
						curChunk = curChunk.lock()->FrontChunk;
						blockPos.z = 0;
					}
					else
					{
						curChunk = curChunk.lock()->BackChunk;
						blockPos.z = 15;
					}
					if (Chunk::IsEmptyChunk(curChunk))
						return block;
				}
			}
		}
		else {
			//y���� ��Ҵٴ� ��
			if (tMaxY < tMaxZ) {
				blockPos.y += stepY;
				fDistance = tMaxY;
				tMaxY += tDeltaY;
				if (blockPos.y >= CHUNK_Y || blockPos.y < 0)
				{
					//��ã��
					return block;
				}
			}
			//z������Ҵٴ� ��
			else {
				blockPos.z += stepZ;
				fDistance = tMaxZ;
				tMaxZ += tDeltaZ;
				if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
				{
					if (stepZ > 0)
					{
						curChunk = curChunk.lock()->FrontChunk;
						blockPos.z = 0;
					}
					else
					{
						curChunk = curChunk.lock()->BackChunk;
						blockPos.z = 15;
					}
					if (Chunk::IsEmptyChunk(curChunk))
						return block;
				}
			}
		}
		fDistance = fDistance < 0 ? fDistance * -1 : fDistance;
		block = curChunk.lock()->GetBlock(blockPos.x, blockPos.y, blockPos.z);
	}

	if (block.blockType != Air)
		outPosition = glm::vec3(blockPos.x + curChunk.lock()->position.x * CHUNK_X, blockPos.y, blockPos.z + curChunk.lock()->position.z * CHUNK_Z);

	return block;
}