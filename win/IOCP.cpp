// IOCP.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include "AsynSocketChannel.h"
//#ifndef iocptcpclient_h__
//#define iocptcpclient_h__
//#include <Winsock2.h>
//#include <windows.h>
//#include <MSTCPiP.h>
//
//#pragma comment(lib, "Ws2_32.lib")
//
//namespace iocp
//{
//	template<typename T>
//	class Scheduler
//	{
//	public:
//		void start()
//		{
//			iocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, scheds);
//			if (NULL == iocp)
//			{
//				throw (int)::WSAGetLastError();
//			}
//		}
//
//		void stop()
//		{
//
//		}
//
//		void push(T * clt)
//		{
//			::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
//		}
//
//	public:
//		int scheds;
//		HANDLE iocp;
//	};
//
//
//	template<typename T>
//	class Processor
//	{
//	public:
//		void start()
//		{
//			for (int i = 0; i < threads; i++)
//			{
//				DWORD tid;
//				HANDLE thd = ::CreateThread(NULL,
//					0,
//					(LPTHREAD_START_ROUTINE)run,
//					this,
//					0,
//					&tid);
//				if (NULL == thd)
//				{
//					throw (int)::GetLastError();
//				}
//				::CloseHandle(thd);
//			}
//		}
//
//		void stop()
//		{
//
//		}
//
//	public:
//
//		static DWORD WINAPI run(LPVOID param)
//		{
//			Processor<T>& procor = *(Processor<T> *)param;
//			Scheduler<T>& scheder = *procor.scheder;
//			HANDLE iocp = scheder.iocp;
//
//			DWORD ready;
//			ULONG_PTR key;
//			WSAOVERLAPPED * overlap;
//			while (true)
//			{
//				::GetQueuedCompletionStatus(iocp, &ready, &key, (LPOVERLAPPED *)&overlap, INFINITE);
//
//				T * clt = (T *)key;
//				switch (clt->event)
//				{
//				case T::EV_RECV:
//				{
//					if (0 >= ready)
//					{
//						clt->event = T::EV_DISCONNECT;
//						::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
//					}
//					else
//					{
//						clt->OnRecv(ready);
//					}
//				}
//					break;
//				case T::EV_CONNECT:
//				{
//					int Connect_Time;
//					int len = sizeof(Connect_Time);
//					int result = getsockopt(clt->fd, SOL_SOCKET, 0x700C/*SO_CONNECT_TIME*/, (char*)&Connect_Time, &len);
//					if (Connect_Time == -1){
//						if (GetTickCount() - clt->dwConnTime >= clt->maxConnTime){
//							clt->OnConnectFailed();
//							::closesocket(clt->fd);
//							clt->fd = INVALID_SOCKET;
//						}
//						else
//						{
//							Sleep(1);
//							::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)clt, NULL);
//						}
//					}
//					else
//					{
//						unsigned long ul = 0;
//						ioctlsocket(clt->fd, FIONBIO, &ul); //设置为阻塞模式*/
//						if (NULL == ::CreateIoCompletionPort((HANDLE)clt->fd, iocp, (ULONG_PTR)clt, 0))
//						{
//							clt->OnConnectFailed();
//							::closesocket(clt->fd);
//							clt->fd = INVALID_SOCKET;
//							//delete clt;
//						}
//						else
//						{
//							clt->OnConnect();
//						}
//					}
//				}
//					break;
//				case T::EV_DISCONNECT:
//				{
//					clt->OnDisconnect();
//					::closesocket(clt->fd);
//					clt->fd = INVALID_SOCKET;
//					//delete clt;
//				}
//					break;
//				case T::EV_SEND:
//					break;
//				}
//			}
//
//			return 0;
//		}
//
//	public:
//
//		int threads;
//
//		Scheduler<T> * scheder;
//	};
//
//	class Client
//	{
//	public:
//		enum EVENT
//		{
//			EV_CONNECT,
//			EV_DISCONNECT,
//			EV_RECV,
//			EV_SEND
//		};
//
//		Client(){
//			fd = INVALID_SOCKET;
//			maxConnTime = 5000;
//		}
//		virtual ~Client(){};
//
//		int connect(){
//			this->event = EV_CONNECT;
//			dwConnTime = GetTickCount();
//			struct sockaddr_in addr;
//			addr.sin_family = AF_INET;
//			addr.sin_addr.s_addr = ip;
//			addr.sin_port = htons(port);
//
//			DWORD dwError = 0, dwBytes = 0;
//			tcp_keepalive sKA_Settings = { 0 }, sReturned = { 0 };
//			sKA_Settings.onoff = 1;
//			sKA_Settings.keepalivetime = 30000;    // Keep Alive in 30 sec.
//			sKA_Settings.keepaliveinterval = 3000; // Resend if No-Reply
//			if (WSAIoctl(fd, SIO_KEEPALIVE_VALS, &sKA_Settings,
//				sizeof(sKA_Settings), &sReturned, sizeof(sReturned), &dwBytes,
//				NULL, NULL) != 0)
//			{
//				dwError = WSAGetLastError();
//			}
//			unsigned long ul = 1;
//			ioctlsocket(fd, FIONBIO, &ul); //设置为非阻塞模式
//			int ret = -1;
//			if (::connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
//			{
//				ret = 0;
//			}
//			return ret;
//		}
//
//		void send(const char * buff, int len){
//			::send(fd, buff, len, 0);
//		}
//
//		void recv(char * buff, int len){
//			this->event = EV_RECV;
//
//			::memset(&overlap, 0, sizeof(overlap));
//			WSABUF buf;
//			buf.buf = buff;
//			buf.len = len;
//			DWORD ready = 0;
//			DWORD flags = 0;
//			if (0 != ::WSARecv(fd, &buf, 1, &ready, &flags, &overlap, NULL)
//				&& WSA_IO_PENDING != WSAGetLastError())
//			{
//				this->event = EV_DISCONNECT;
//				::PostQueuedCompletionStatus(iocp, 0, (ULONG_PTR)this, NULL);
//			}
//		}
//		void close(){
//			::shutdown(fd, SD_BOTH);
//		}
//
//		virtual void OnConnect(){};//连接成功
//
//		virtual void OnConnectFailed(){};//连接失败
//
//		virtual void OnDisconnect(){};	//连接断开
//
//		virtual void OnRecv(int len){};
//
//		virtual void OnSend(int len){};
//	public:
//		int ip;
//		int port;
//		void * srv;
//		HANDLE iocp;
//		EVENT event;
//		SOCKET fd;
//		DWORD maxConnTime;
//		DWORD dwConnTime;
//		WSAOVERLAPPED overlap;
//	};
//
//	template<typename T>
//	class TCPClt
//	{
//	public:
//		void start()
//		{
//			WSADATA wsadata;
//			int wsaversion = WSAStartup(MAKEWORD(2, 2), &wsadata);
//
//			if (threads <= 0)
//			{
//				threads = 1;
//			}
//
//			scheder.scheds = scheds;
//			scheder.start();
//
//			procor.threads = threads;
//			procor.scheder = &scheder;
//			procor.start();
//		}
//
//		void stop()
//		{
//		}
//
//		bool addclt(T* clt, int ip, int port)
//		{
//			clt->ip = ip;
//			clt->port = port;
//			clt->iocp = scheder.iocp;
//			clt->fd = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
//			if (clt->fd == INVALID_SOCKET)
//			{
//				return false;
//			}
//			if (clt->connect() != 0)
//			{
//				closesocket(clt->fd);
//				return false;
//			}
//			scheder.push(clt);
//			return true;
//		}
//
//	public:
//		int scheds;
//		int threads;
//
//		iocp::Scheduler<T> scheder;
//		iocp::Processor<T> procor;
//	};
//}
//#endif // iocptcpclient_h__



class TestIOCP
{
public:

	void run()
	{
		m_readHandler = [this](unsigned char* buffer, int size)
		{
			if (this->file)
			{
				fwrite(buffer, 1, size, this->file);
				ftell(this->file);
			}
			channel->read(buffer, 8192, m_readHandler);
		};

		channel = AsynSocketChannel::open();
		

		channel->connect("127.0.0.1", 8080, [this](bool success)
		{
			if (success)
			{		
				char szFile[128];
				sprintf(szFile, "D:\\test\\index_%d.html", fileNum);
				this->file = fopen(szFile, "wb");

				char* szHeader = "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: Keep-Alive\r\n\r\n";
				channel->write((unsigned char*)szHeader, strlen(szHeader));
				channel->read(buffer, 8192, m_readHandler);
			}
			else
			{
				std::cout << "connect faile " << this->fileNum;
			}
		});	
	}

	void runBaidu()
	{
		m_readHandler = [this](unsigned char* buffer, int size)
		{
			if (this->file)
			{
				fwrite(buffer, 1, size, this->file);
				ftell(this->file);
			}
			channel->read(buffer, 8192, m_readHandler);
		};

		channel = AsynSocketChannel::open();


		channel->connect("192.168.8.54", 8899, [this](bool success)
		{
			if (success)
			{
				char szFile[128];
				sprintf(szFile, "D:\\test\\index_baidu_%d.html", fileNum);
				this->file = fopen(szFile, "wb");
				//http://192.168.8.54:8899/release/video/%E8%87%B4%E9%82%A3%E8%8D%92%E8%AF%9E%E7%94%9F%E6%B4%BB.zip
				char* szHeader = "GET /release/video/%E8%87%B4%E9%82%A3%E8%8D%92%E8%AF%9E%E7%94%9F%E6%B4%BB.zip HTTP/1.1\r\nHost: 192.168.8.54:8899\r\nConnection: Keep-Alive\r\n\r\n";
				channel->write((unsigned char*)szHeader, strlen(szHeader));
				if (readCount <= 10)
					channel->read(buffer, 8192, m_readHandler);
				readCount++;
			}
			else
			{
				std::cout << "connect faile " << this->fileNum;
			}
		});
	}

public:

	FILE* file;

	int fileNum;

	int readCount = 0;

	std::function<void(unsigned char*, int)> m_readHandler;

	AsynSocketChannel*channel;

	unsigned char* buffer = new unsigned char[8192];
};

#include <vector>
using namespace std;
int _tmain(int argc, _TCHAR* argv[])
{
	int count = 1;
	std::vector<TestIOCP*> listIOCP;
	for (int i = 0; i < count; i++)
	{
		TestIOCP* test = new TestIOCP();
		listIOCP.push_back(test);
		test->fileNum = i;

		//if (i % 2 == 0)
		//	test->run();
		//else
			test->runBaidu();
	}

	int i = 0;
	std::cin >> i;

	for (int j = 0; j < count; j++)
	{
		TestIOCP* iocp = listIOCP[j];
		fclose(iocp->file);
	}
	return 0;
}

