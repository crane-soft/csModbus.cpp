#pragma once

#include<winsock2.h>
#include<Ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#include "TcpClient.h"
#include <stdint.h>
#include <thread>
#include <list>


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

class ClientList {
public:
	ClientList();
	~ClientList();

	void SetMaster(SOCKET master);
	void fill_fds();
	fd_set * get_fds() { return &readfds;	}
	bool isMaster();
	void addClient(NetStream *client);
	void setFirst();
	NetStream * getClient();
	void removeClient();
	void deleteClients();
private:
	SOCKET masterSock;
	fd_set readfds;
	
	std::list <NetStream*> clients;
	typedef std::list <NetStream*>::iterator client_it;
	client_it currentClient, nextClient;
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
	ClientList clientList;
};
