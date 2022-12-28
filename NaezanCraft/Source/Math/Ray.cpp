#include "../../pch.h"
#include "../../World/Block.h"
#include "../../World/Chunk.h"
#include "../Application.h"
#include "Ray.h"

Block Ray::BlockTraversal(const glm::vec3& ray_start, const glm::vec3& dir)
{
    std::weak_ptr<Chunk> curChunk;
    GET_World()->GetChunkByPos(std::make_pair(static_cast<int>(ray_start.x / CHUNK_X), static_cast<int>(ray_start.z / CHUNK_Z)), curChunk);
    glm::vec3 blockPos;
    //����ó��
    blockPos.x = static_cast<int>(ray_start.x) % CHUNK_X;
    blockPos.y = static_cast<int>(ray_start.y) % CHUNK_Y;
    blockPos.z = static_cast<int>(ray_start.z) % CHUNK_Z;

	float stepX = dir.x >= 0 ? 1 : -1;
	float stepY = dir.y >= 0 ? 1 : -1;
	float stepZ = dir.z >= 0 ? 1 : -1;

    //������ �˻��� ��
	float nextX = (std::round(ray_start.x) + stepX) * CHUNK_X;
	float nextY = (std::round(ray_start.y) + stepY) * CHUNK_Y;
	float nextZ = (std::round(ray_start.z) + stepZ) * CHUNK_Z;

	float tMaxX = (nextX - ray_start.x) / dir.x;//dir�� ������ ������ ����? x�� ��ô������ �����ִ°� ������ �⺻���� ���꺸�� ���� ���δ�
	float tMaxY = (nextY - ray_start.y) / dir.y;
	float tMaxZ = (nextZ - ray_start.z) / dir.z;

	float tDeltaX = CHUNK_X / dir.x * stepX;
	float tDeltaY = CHUNK_Y / dir.y * stepY;
	float tDeltaZ = CHUNK_Z / dir.z * stepZ;

    //RayBlock ���࿡ ���������� ����ų� ���� ã���� �ݺ����� �����´�
    Block block(BlockType::Air);
    while (block.IsFluid() /*&& glm::length("distance") < 10.f*/) {
        if (tMaxX < tMaxY) {
            //x�� ���� �۴ٴ� �� x���� ���� ��Ҵٴ� ��
            if (tMaxX < tMaxZ) {
                blockPos.x += stepX;
                tMaxX += tDeltaX;
                if (blockPos.x >= CHUNK_X || blockPos.x < 0)
                {
                    //x���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
                }
            }
            //z���� ��Ҵٴ� ��
            else {
                blockPos.z += stepZ;
                tMaxZ += tDeltaZ;
                if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
                {
                    //z���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
                }
            }
        }
        else {
            //y���� ��Ҵٴ� ��
            if (tMaxY < tMaxZ) {
                blockPos.y += stepY;
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
                tMaxZ += tDeltaZ;
                if (blockPos.z >= CHUNK_Z || blockPos.z < 0)
                {
                    //z���⿡ �ִ� ����ûũ�� ����, ����ûũ�� ������ return block
                }
            }
        }

        block = curChunk.lock()->GetBlock(blockPos.x, blockPos.y, blockPos.z);
    }

    return block;
}