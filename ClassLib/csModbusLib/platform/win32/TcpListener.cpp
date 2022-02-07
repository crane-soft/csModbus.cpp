
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
	clientList.deleteClients();
}

void TcpListener::HandleClientRequest()
{
	clientList.SetMaster(tcpSocket);

	while (running) {
		clientList.fill_fds();
		int activity = select(0, clientList.get_fds(), NULL, NULL, NULL);
		if (activity == SOCKET_ERROR) {
			int wsaError = WSAGetLastError();
			DebugPrint("WSA-error %d", wsaError);
			break;
		}
			
		if (clientList.isMaster()) {
			acceptNewConnection();
		} else {
			clientList.setFirst();
			NetStream *client;
			while ((client = clientList.getClient()) != 0) {
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
		clientList.addClient(client);
		OnClientAccepted(client);
	}
}

void TcpListener::HanndleConnection(NetStream *client)
{
	int retCode = client->ClientRead();
	OnClientRead(client->GetContext());
	if (retCode == 0) {
		clientList.removeClient();
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
// Class ClientList
//-----------------------------------------------------------------------
ClientList::ClientList() {
}

ClientList::~ClientList()
{
	deleteClients();
}

void ClientList::SetMaster(SOCKET master) {
	masterSock = master;
}

bool ClientList::isMaster() {
	return (FD_ISSET(masterSock, &readfds) != 0);
}

void ClientList::fill_fds()
{
	FD_ZERO(&readfds);
	FD_SET(masterSock, &readfds);

	for (NetStream *cl : clients) {
		FD_SET(cl->getSock(), &readfds);
	}
}

void ClientList::addClient(NetStream *client)
{
	clients.push_back(client);
}

void ClientList::removeClient()
{
	if (currentClient != clients.end()) {
		delete *currentClient;
		clients.erase(currentClient);
	}
}

void ClientList::setFirst()
{
	nextClient = clients.begin();
}

NetStream *ClientList::getClient()
{
	for (auto it = nextClient; it != clients.end(); ++it) {
		++nextClient;
		NetStream *cl = *it;
		if (FD_ISSET(cl->getSock(), &readfds)) {
			currentClient = it;
			return cl;
		}
	}

	currentClient = clients.end();
	return 0;
}

void ClientList::deleteClients()
{
	for (NetStream *cl : clients) {
		delete cl;
	}
	clients.clear();
	currentClient = clients.end();
}
