#pragma once
#pragma once
#ifdef WIN32
#include<winsock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#endif

class NetSocket {
public:
	NetSocket();
	~NetSocket();

	void SetReceiveTimeout(int timeout);

private:
#ifdef WIN32
	int wsaResult;
	SOCKET Socket;
#endif
};
