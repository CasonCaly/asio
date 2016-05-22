#ifndef AYSN_CHANNEL_GROUP_H
#define AYSN_CHANNEL_GROUP_H
#include <thread>
#include <map>
#include <list>
using namespace std;

#include "AsynSocketChannel.h"


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

	int m_kqueue;

	std::map<int, AsynSocketChannel*> m_keyToChannel;

protected:

	static AsynChannelGroup* s_instance;
};

#endif