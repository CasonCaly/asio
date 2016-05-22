#include <string>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "AsynChannelGroup.h"

AsynSocketChannel* AsynSocketChannel::open()
{
	AsynChannelGroup* group = AsynChannelGroup::getInstance();
	AsynSocketChannel* socketChannel = new AsynSocketChannel();
	group->associate(socketChannel);
	return socketChannel;
}

AsynSocketChannel::AsynSocketChannel()
{
	m_readTimeOut = 10 * 1000;
	m_connectTimeOut = 15 * 1000;
    m_socket = ::socket(PF_INET, SOCK_STREAM, 0);
    int flags = ::fcntl(m_socket, F_GETFL);
    
    int newflags = flags | O_NONBLOCK;
    ::fcntl(m_socket, F_SETFL, newflags);
    
}

void AsynSocketChannel::connect(const char* szHost, int port, const std::function<void(bool)>& handler)
{
	m_conncetHandler = handler;
	sockaddr_in addrPeer;
	memset(&addrPeer, 0, sizeof (sockaddr_in));
	addrPeer.sin_family = AF_INET;
	addrPeer.sin_addr.s_addr = inet_addr(szHost);
	addrPeer.sin_port = htons(port);
	int nLen = sizeof (addrPeer);
    struct kevent connectChange[1];
    EV_SET(&connectChange[0], , <#b#>, <#c#>, <#d#>, <#e#>, <#f#>)
    
    kevent(<#int kq#>, <#const struct kevent *changelist#>, <#int nchanges#>, <#struct kevent *eventlist#>, <#int nevents#>, <#const struct timespec *timeout#>)
}

void AsynSocketChannel::write(unsigned char* byteData, int size)
{
	WSABUF wsaBuf;
	wsaBuf.buf = (CHAR*)byteData;
	wsaBuf.len = size;

	AsynChannelGroup* group = AsynChannelGroup::getInstance();
	OVERLAPPED_EX*  pOverlapped = group->m_overlappedCache.add(this, PendingWrite);
	int send = ::WSASend(m_socket, &wsaBuf, 1, NULL, 0, (LPOVERLAPPED)pOverlapped, NULL);
	if (send == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			printf("diss write error");
		}
	}
}

void AsynSocketChannel::read(unsigned char* buffer, int bufferSize, const std::function<void(unsigned char*, int)>& handler)
{
	WSABUF wsaBuf;
	wsaBuf.buf = (CHAR*)buffer;
	wsaBuf.len = bufferSize;

	m_readHandler = handler;
	AsynChannelGroup* group = AsynChannelGroup::getInstance();
	OVERLAPPED_EX* readOverlapped = group->m_overlappedCache.add(this, PendingRead);
	readOverlapped->buffer = wsaBuf.buf;
	DWORD flags = 0;
	int recv = ::WSARecv(m_socket, &wsaBuf, 1, NULL, &flags, (OVERLAPPED*)readOverlapped, NULL);
	if (SOCKET_ERROR == recv)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			printf("diss read error");
		}
	}
}