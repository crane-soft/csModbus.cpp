#pragma once

#include<winsock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "TcpClient.h"
#include <stdint.h>
#include <thread>


#define MAX_CLIENTS	10

class NetStream {
public:
	NetStream(SOCKET aSock);
	~NetStream();
	int Send(const uint8_t *Data, int Length);
	void BeginRcv(uint8_t *Data, int maxLength, void * context);
	int EndRead() { return RcvReturn; }
	SOCKET getSock() { return Sock; }
	int ClientRead();
	void *GetContext() { return RcvContext; }
private:
	void  *RcvContext;
	uint8_t *RcvData;
	uint8_t RcvLengt;
	int RcvReturn;
	SOCKET Sock;
};

class SocketSet {
public:
	SocketSet();
	~SocketSet();

	void SetMaster(SOCKET master);
	void fill_fd();
	fd_set * get() { return &readfds;	}
	bool isMaster();
	bool add(NetStream *client);
	NetStream * getClient();
	void removeClient();
	void deleteClients();
private:
	SOCKET masterSock;
	fd_set readfds;
	int ClientIdx;
	NetStream *clients[MAX_CLIENTS];
	void clearClients();
	void deleteClient(int i);
};

class TcpListener {
public:
	TcpListener();
	~TcpListener();

	void StartListen(int port);
	void Stop();

protected:
	void virtual OnClientAccepted(NetStream *stream) {}
	void virtual OnClientRead(void * Context) {}

private:
	void acceptNewConnection();
	void HandleClientRequest();
	void HanndleConnection(NetStream *client);

	std::thread *ListenThread = NULL;
	bool running = false;
	int wsaResult;
	SOCKET tcpSocket;
	SocketSet FdSet;
	char buffer[MAX_BUFFER];
};
