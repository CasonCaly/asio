#include "AsynChannelGroup.h"
#include <Winsock2.h>
#include <windows.h>
#include <MSTCPiP.h>

#ifndef WSAID_CONNECTEX
#define WSAID_CONNECTEX {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}
#endif

#ifndef SO_UPDATE_CONNECT_CONTEXT
#define SO_UPDATE_CONNECT_CONTEXT 0x7010
#endif

typedef BOOL(PASCAL *ConnectEx_t)
(
	SOCKET s,
	const struct sockaddr* name,
	int namelen,
	PVOID lpSendBuffer,
	DWORD dwSendDataLength,
	LPDWORD lpdwBytesSent,
	LPOVERLAPPED lpOverlapped
);

static ConnectEx_t ConnectEx = NULL;

void initConnectExFun()
{
	if (NULL == ConnectEx)
	{
		WORD wVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		WSAStartup(wVersion, &wsaData);

		GUID GuidConnectEx = WSAID_CONNECTEX;
		SOCKET s;
		int rv;
		DWORD dwBytes;

		s = socket(AF_INET, SOCK_STREAM, 0);
		rv = WSAIoctl(s,
			SIO_GET_EXTENSION_FUNCTION_POINTER,
			(LPVOID)&GuidConnectEx,
			sizeof(GuidConnectEx),
			&ConnectEx,
			sizeof(ConnectEx),
			&dwBytes,
			NULL,
			NULL);
		::closesocket(s);
	}
}

AsynSocketChannel* AsynSocketChannel::open()
{
	::initConnectExFun();
	AsynChannelGroup* group = AsynChannelGroup::getInstance();
	AsynSocketChannel* socketChannel = new AsynSocketChannel();
	group->associate(socketChannel);
	return socketChannel;
}

AsynSocketChannel::AsynSocketChannel()
{
	m_readTimeOut = 10 * 1000;
	m_connectTimeOut = 15 * 1000;
	m_socket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
}

void AsynSocketChannel::connect(const char* szHost, int port, const std::function<void(bool)>& handler)
{
	m_conncetHandler = handler;
	sockaddr_in addrPeer;
	ZeroMemory(&addrPeer, sizeof (sockaddr_in));
	addrPeer.sin_family = AF_INET;
	addrPeer.sin_addr.s_addr = inet_addr(szHost);
	addrPeer.sin_port = htons(port);
	int nLen = sizeof (addrPeer);

	AsynChannelGroup* group = AsynChannelGroup::getInstance();
	OVERLAPPED_EX*  pOverlapped = group->m_overlappedCache.add(this, PendingConnect);
	int ret = ::ConnectEx(m_socket, (sockaddr *)&addrPeer, nLen, NULL, 0, NULL, (LPOVERLAPPED)pOverlapped);
	if (ret == 0)
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			printf("diss error");
		}
	}
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