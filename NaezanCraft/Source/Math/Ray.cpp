#include "../../pch.h"
#include "../../World/Block.h"
#include "../../World/Chunk.h"
#include "../Application.h"
#include "Ray.h"

Block Ray::BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir, glm::vec3& outPosition)
{
	std::weak_ptr<Chunk> curChunk;
	Block block(BlockType::Air);
	GET_World()->GetChunkByPos(std::make_pair(static_cast<int>(ray_start.x / CHUNK_X), static_cast<int>(ray_start.z / CHUNK_Z)), curChunk);

	if (Chunk::IsEmptyChunk(curChunk))
		return block;

	glm::vec3 blockPos;
	//ûũ�� �� ������ġ(����ó��)
	blockPos.x = static_cast<int>(ray_start.x) % CHUNK_X;
	blockPos.y = static_cast<int>(ray_start.y) % CHUNK_Y;
	blockPos.z = static_cast<int>(ray_start.z) % CHUNK_Z;

	float stepX = dir.x >= 0 ? 1 : -1;
	float stepY = dir.y >= 0 ? 1 : -1;
	float stepZ = dir.z >= 0 ? 1 : -1;

	//������ �˻��� �� ���� ��ġ
	float nextX = (blockPos.x + stepX) + curChunk.lock()->position.x * CHUNK_X;
	float nextY = (blockPos.y + stepY);
	float nextZ = (blockPos.z + stepZ) + curChunk.lock()->position.z * CHUNK_Z;

	float tMaxX = (nextX - ray_start.x) / dir.x;//��������ġ - ������ ������ġ
	float tMaxY = (nextY - ray_start.y) / dir.y;
	float tMaxZ = (nextZ - ray_start.z) / dir.z;

	float tDeltaX = stepX / dir.x;//��ĭ �����Ҷ��� �밢���� ���� �ܼ��� dir�� ������ �̷�����ٴ°� ����� �����غ��� �� �� �ִ�.
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
					//x���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
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
					//z���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
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
					//z���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
					return block;
				}
			}
		}

		block = curChunk.lock()->GetBlock(blockPos.x, blockPos.y, blockPos.z);
	}

	if (block.blockType != Air)
		outPosition = glm::vec3(blockPos.x + curChunk.lock()->position.x * CHUNK_X, blockPos.y, blockPos.z + curChunk.lock()->position.z * CHUNK_Z);

	return block;
}