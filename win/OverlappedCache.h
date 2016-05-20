#ifndef OVERLAPPED_CACHE_H
#define OVERLAPPED_CACHE_H
#include <list>
#include <mutex>
#include <set>
using namespace std;

#include <Winsock2.h>
#include <windows.h>
#include <MSTCPiP.h>

class AsynSocketChannel;
class AsynChannelGroup;

typedef enum 
{
	PendingConnect,
	PendingRead,
	PendingWrite,
	PendingCustom
}PendingTaskType;

typedef struct
{
	OVERLAPPED overlapped;
	CHAR* buffer;

	AsynSocketChannel* channel;
	PendingTaskType taskType;
}OVERLAPPED_EX;

class OverlappedCache
{
	friend class AsynChannelGroup;

	friend class AsynSocketChannel;

protected:

	OverlappedCache();

	OVERLAPPED_EX* add(AsynSocketChannel* channel, PendingTaskType type);

	void remove(OVERLAPPED_EX* overlapped);

protected:

	std::set<OVERLAPPED_EX*> m_cache;

	std::list<OVERLAPPED_EX*> m_unusedCache;

	std::mutex m_lock;
};

#endif