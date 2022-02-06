#pragma once
#ifdef WIN32
#include<winsock2.h>
#include<Ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
typedef int socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define SOCKET_ERROR -1
#endif
#include <stdint.h>

#define MAX_CLIENTS	10
#define MAX_BUFFER 1024

class TcpClient {
public:
	TcpClient();
	~TcpClient();

	void Connect(const char* hostname, int port);
	void Close();

	int Send(const uint8_t *Data, int Length);
	int Receive(uint8_t *Data, int maxLength);
	void SetReceiveTimeout(int timeout);

private:
	int wsaResult;
	SOCKET tcpSocket;
};
