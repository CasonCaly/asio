#include "AsynChannelGroup.h"
#include <iostream>

AsynChannelGroup* AsynChannelGroup::s_instance = nullptr;

AsynChannelGroup* AsynChannelGroup::getInstance()
{
	if (nullptr == s_instance)
		s_instance = new AsynChannelGroup();
	return s_instance;
}

AsynChannelGroup::AsynChannelGroup()
{
	m_nextCompletionKey = 0;
	m_thread = std::thread(&AsynChannelGroup::run, this);
	m_thread.detach();
	m_iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
}

void AsynChannelGroup::associate(AsynSocketChannel* socketChannel)
{
	SOCKET clientSocket = socketChannel->m_socket;
	sockaddr_in localAddr;
	ZeroMemory(&localAddr, sizeof(sockaddr_in));
	localAddr.sin_family = AF_INET;

	m_nextCompletionKey++;
	m_keyToChannel.insert(make_pair(m_nextCompletionKey, socketChannel));

	::bind(clientSocket, (sockaddr*)(&localAddr), sizeof(sockaddr_in));
	::CreateIoCompletionPort((HANDLE)clientSocket, m_iocp, (ULONG_PTR)m_nextCompletionKey, 0);
}

void AsynChannelGroup::wakeup()
{
	::PostQueuedCompletionStatus(m_iocp, 0, 0, NULL);
}

void AsynChannelGroup::run()
{
	DWORD bytesTransferred = 0;
	ULONG_PTR completionKey = 0;
	WSAOVERLAPPED* overlap = NULL;

	while (true)
	{
		BOOL result = ::GetQueuedCompletionStatus(m_iocp, &bytesTransferred, &completionKey, (LPOVERLAPPED *)&overlap, INFINITE);
		DWORD ioResult = (result == 0) ? GetLastError() : 0;
		//表示被唤醒的任务
		if (NULL == overlap && 0 == completionKey)
			continue;

		auto target = m_keyToChannel.find(completionKey);
		if (target == m_keyToChannel.end())
			continue;

		OVERLAPPED_EX* overlppedEx = (OVERLAPPED_EX*)overlap;
		AsynSocketChannel* channel = target->second;
		SOCKET socket = channel->m_socket;

		switch (overlppedEx->taskType)
		{
			case PendingConnect:
			{
				if (channel->m_conncetHandler)
					channel->m_conncetHandler(result == TRUE);
				break;
			}
			case PendingWrite:
			{
				break;
			}
			case PendingRead:
			{
				if (0 != bytesTransferred)
				{
					if (channel->m_readHandler)
						channel->m_readHandler((unsigned char*)overlppedEx->buffer, bytesTransferred);
				}
				else
				{
					std::cout << "read " << ioResult <<endl;
				}
				
				break;
			}
		}
		m_overlappedCache.remove(overlppedEx);
	}
}