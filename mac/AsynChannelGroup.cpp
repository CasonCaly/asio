#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "AsynChannelGroup.h"
#include <iostream>
const int MAX_EVENT_COUNT = 5000;

AsynChannelGroup* AsynChannelGroup::s_instance = nullptr;

AsynChannelGroup* AsynChannelGroup::getInstance()
{
	if (nullptr == s_instance)
		s_instance = new AsynChannelGroup();
	return s_instance;
}

AsynChannelGroup::AsynChannelGroup()
{
	m_thread = std::thread(&AsynChannelGroup::run, this);
	m_thread.detach();
    m_kqueue = ::kqueue();
}

void AsynChannelGroup::associate(AsynSocketChannel* socketChannel)
{
	int clientSocket = socketChannel->m_socket;
    
    struct kevent changes[1];
    struct timespec timeout = {0, 0};
    
    EV_SET(&changes[0], clientSocket, EVFILT_WRITE, EV_ADD|EV_ONESHOT, 0, 0, 0);
    int res = ::kevent(m_kqueue, &changes[0], 1, NULL, 0, &timeout);
	m_keyToChannel.insert(make_pair(clientSocket, socketChannel));

}

void AsynChannelGroup::wakeup()
{
	
}

void AsynChannelGroup::run()
{
	struct kevent events[MAX_EVENT_COUNT];

	while (true)
	{
        int ret = ::kevent(m_kqueue, NULL, 0, events, MAX_EVENT_COUNT, NULL);
		
        for(int i = 0; i < ret; i++)
        {
            struct kevent& curEvent = events[i];
            int socket = (int)curEvent.ident;
            int filter = curEvent.filter;
            
            auto target = m_keyToChannel.find(socket);
            if (target == m_keyToChannel.end())
                continue;
            
            AsynSocketChannel* channel = target->second;
            
            switch (filter)
            {
                case EVFILT_WRITE :
                {
                    if (channel->m_conncetHandler)
                        channel->m_conncetHandler(true);
                    break;
                }
                case EVFILT_READ:
                {
                    channel->mb
                    if (channel->m_readHandler)
                        channel->m_readHandler((unsigned char*)overlppedEx->buffer, bytesTransferred);
                    
                    break;
                }
            }

        }
        
//		m_overlappedCache.remove(overlppedEx);
	}
}