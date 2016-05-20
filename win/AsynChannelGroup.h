#ifndef AYSN_CHANNEL_GROUP_H
#define AYSN_CHANNEL_GROUP_H
#include <thread>
#include <map>
#include <list>
using namespace std;

#include "AsynSocketChannel.h"
#include "OverlappedCache.h"

#pragma comment(lib, "Ws2_32.lib")

class AsynChannelGroup
{
	friend class AsynSocketChannel;

protected:

	static AsynChannelGroup* getInstance();

	AsynChannelGroup();

protected:

	void associate(AsynSocketChannel* socketChannel);

	void wakeup();

	void run();

protected:

	std::thread m_thread;

	HANDLE m_iocp;

	std::map<int, AsynSocketChannel*> m_keyToChannel;

	OverlappedCache m_overlappedCache;

	int m_nextCompletionKey;

protected:

	static AsynChannelGroup* s_instance;
};

#endif