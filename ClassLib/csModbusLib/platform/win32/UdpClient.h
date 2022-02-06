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
class UdpClient {
public:
	UdpClient();
	~UdpClient();
	UdpClient(const char* hostname, int port);
	void Connect(const char* hostname, int port);
	void Close();
	int Send(const uint8_t *Data, int Length);
	int SendResponse(const uint8_t *Data, int Length);

	int Receive(uint8_t *Data, int maxLength);
	void SetReceiveTimeout(int timeout);

private:
	void InitSocket();
	int upd_sendto(const uint8_t *Data, int Length, sockaddr_in *addr);

#ifdef WIN32
	int wsaResult;
	SOCKET udpSocket;
#else
	int sock;
#endif
	sockaddr_in outaddr;
	sockaddr_in remote_addr;
};
