#include "pch.h"
#include "ChunkThread.h"
#include "World/Chunk.h"
#include "World/World.h"

std::vector<std::weak_ptr<Chunk>> ChunkThread::saveChunks;

ChunkThread::ChunkThread() :
	IsThreadRunning(true)
{
	chunkThread = std::thread(&ChunkThread::RunChunkThread, this);

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string::size_type pos = std::string(buffer).find_last_of("\\/");


}

void ChunkThread::RunChunkThread()
{
	bool loopCondition = true;
	while (loopCondition)
	{
		//if(save command is empty)
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

		if (!saveChunks.empty())
		{
			SerializeChunk();

			chunkCV.notify_one();
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
