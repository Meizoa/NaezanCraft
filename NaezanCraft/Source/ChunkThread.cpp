#include "pch.h"
#include "ChunkThread.h"
#include "World/Chunk.h"
#include "World/World.h"

ChunkThread::ChunkThread() :
	IsThreadRunning(true)
{
	chunkThread = std::thread(&ChunkThread::RunChunkThread, this);
}

ChunkThread::~ChunkThread()
{
	chunkCV.notify_all();

	chunkThread.join();
}

void ChunkThread::RunChunkThread()
{
	bool loopCondition = true;
	while (loopCondition)
	{
		if (saveChunks.empty())
		{
			std::unique_lock<std::mutex> lock(chunkMutex);
			if (IsThreadRunning)
			{
				//�����尡 �������̶�� Ư�������� �����ϱ������� ������ ����մϴ�.
				//pred�� true��� wait�� ����ϴ�
				chunkCV.wait(lock, [&] { return !saveChunks.empty(); });
				loopCondition = true;
			}
			else
			{
				loopCondition = false;
			}
		}

		if (!saveChunks.empty())
		{
			SerializeChunk();

			chunkCV.notify_all();
		}
	}
}

void ChunkThread::SerializeChunk()
{
	auto iter = saveChunks.begin();
	for (; iter != saveChunks.end();)
	{
		iter->lock()->SaveChunk(World::worldPath);
		iter = saveChunks.erase(iter);
	}
}

void ChunkThread::BeginThread()
{
	chunkCV.notify_all();
}
