
#include "platform.h"
#include "MbSlave.h"
#include "TcpListener.h"

TcpListener::TcpListener()
{
#ifdef WIN32
	WSADATA wsa;
	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
	tcpSocket = SOCKET_ERROR;
}

TcpListener::~TcpListener()
{
#if WIN32
	WSACleanup();
#endif
}

// https://www.binarytides.com/code-tcp-socket-server-winsock/

void TcpListener::StartListen(int port)
{
	if (wsaResult != 0)
		throw WSASYSNOTREADY;

	tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSocket == INVALID_SOCKET)
		throw WSAGetLastError();

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(tcpSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
		throw WSAGetLastError();

	listen(tcpSocket, 3);
	running = true;
	ListenThread = new std::thread(&TcpListener::HandleClientRequest, this);
}

void TcpListener::Stop()
{
	running = false;
	shutdown(tcpSocket, SD_RECEIVE);
	closesocket(tcpSocket);
	if (ListenThread) {
		ListenThread->join();
		delete ListenThread;
	}
	FdSet.deleteClients();
}

void TcpListener::HandleClientRequest()
{
	FdSet.SetMaster(tcpSocket);

	while (running) {
		FdSet.fill_fd();
		int activity = select(0, FdSet.get(), NULL, NULL, NULL);
		if (activity == SOCKET_ERROR) {
			int wsaError = WSAGetLastError();
			DebugPrint("WSA-error %d", wsaError);
			break;
		}
			
		if (FdSet.isMaster()) {
			acceptNewConnection();
		} else {
			NetStream *client = FdSet.getClient();
			if (client != 0) {
				HanndleConnection(client);
			}
		}
	}
}

void TcpListener::acceptNewConnection()
{
	struct sockaddr_in clientAddr;
	int addrlen = sizeof(struct sockaddr_in);
	NetStream *client = 0;
	SOCKET newSocket = accept(tcpSocket, (struct sockaddr *)&clientAddr, (int *)&addrlen);
	if (newSocket != INVALID_SOCKET) {
		client = new NetStream(newSocket);
		FdSet.add(client);
		OnClientAccepted(client);
	}
}

void TcpListener::HanndleConnection(NetStream *client)
{
	int retCode = client->ClientRead();
	OnClientRead(client->GetContext());
	if (retCode == 0) {
		FdSet.removeClient();
	}
}

//-----------------------------------------------------------------------
// Class NetStream
//-----------------------------------------------------------------------
NetStream::NetStream(SOCKET aSock)
{
	this->Sock = aSock;
	RcvLengt = 0;
}
NetStream::~NetStream()
{
	if (Sock != SOCKET_ERROR)
		closesocket(Sock);
}

int NetStream::ClientRead()
{
	int retCode = 0;
	if (RcvLengt > 0) {
		retCode = recv(Sock, (char*)RcvData, RcvLengt, 0);
		RcvLengt = 0;
		if (retCode == SOCKET_ERROR) {
			// TODO handle errors instead of close connection
			retCode = 0;
		}
	}
	RcvReturn = retCode;
	return retCode;
}

int NetStream::Send(const uint8_t *Data, int Length)
{
	int retCode = send(Sock, (char*)Data, Length, 0);
	if (retCode < Length) {
		throw csModbusLib::ErrorCodes::TX_TIMEOUT;
	}
	if (retCode == SOCKET_ERROR) {
		int wsaError = WSAGetLastError();
		DebugPrint("WSA-error %d", wsaError);
		throw csModbusLib::ErrorCodes::CONNECTION_ERROR;
	}
	return retCode;
}

void NetStream::BeginRcv(uint8_t *Data, int maxLength, void * context)
{
	RcvContext = context;
	RcvData = Data;
	RcvLengt = maxLength;
}

//-----------------------------------------------------------------------
// Class SocketSet
//-----------------------------------------------------------------------
SocketSet::SocketSet() {
	clearClients();
}

SocketSet::~SocketSet()
{
	deleteClients();
}

void SocketSet::SetMaster(SOCKET master) {
	masterSock = master;
}

bool SocketSet::isMaster() {
	return (FD_ISSET(masterSock, &readfds) != 0);
}

void SocketSet::fill_fd()
{
	FD_ZERO(&readfds);
	FD_SET(masterSock, &readfds);
	for (int i = 0; i < MAX_CLIENTS; i++) {
		NetStream *s = clients[i];
		if (s != 0) {
			FD_SET(s->getSock(), &readfds);
		}
	}
}

bool SocketSet::add(NetStream *client)
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		NetStream *ns = clients[i];
		if (ns == 0) {
			clients[i] = client;
			return true;
		}
	}
	return false;
}

void SocketSet::removeClient()
{
	if (ClientIdx >= 0) {
		deleteClient(ClientIdx);
	}
}

NetStream *SocketSet::getClient()
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		NetStream *ns = clients[i];
		if (FD_ISSET(ns->getSock(), &readfds)) {
			ClientIdx = i;
			return ns;
		}
	}
	ClientIdx = -1;
	return 0;
}

void SocketSet::clearClients()
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		clients[i] = 0;
	}
	ClientIdx = -1;
}

void SocketSet::deleteClients()
{
	for (int i = 0; i < MAX_CLIENTS; i++) {
		deleteClient(i);
	}
	ClientIdx = -1;
}

void SocketSet::deleteClient(int i)
{
	NetStream *ns = clients[i];
	if (ns != 0) {
		delete ns;
	}
	clients[i] = 0;
}
