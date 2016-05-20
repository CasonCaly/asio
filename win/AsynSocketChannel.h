#ifndef ASYN_SOCKET_CHANNEL_H
#define AYSN_SOCKET_CHANNEL_H
#include <functional>

class AsynSocketChannel
{
	friend class AsynChannelGroup;

public:

	static AsynSocketChannel* open();

public:

	void setConnectTimeout(int connectTimeout);

	void setReadTimeout(int readTimeout);

	void connect(const char* szHost, int port, const std::function<void(bool)>& handler);

	void read(unsigned char* buffer, int bufferSize, const std::function<void(unsigned char*, int)>& handler);

	void write(unsigned char* byteData, int size);

protected:

	AsynSocketChannel();

protected:

	int m_readTimeOut;	  //∫¡√Î

	int m_connectTimeOut; //∫¡√Î

	int m_socket;

	std::function<void(unsigned char*, int)> m_readHandler;

	std::function<void(bool)> m_conncetHandler;

};

#endif