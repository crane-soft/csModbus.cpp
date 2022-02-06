
#include "platform.h"
#include "MbSlave.h"
#include "TcpClient.h"

TcpClient::TcpClient()
{
#ifdef WIN32
	WSADATA wsa;
	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}


TcpClient::~TcpClient()
{
#if WIN32
	closesocket(tcpSocket);
	WSACleanup();
#else
	close(sock);
#endif
}

void TcpClient::Connect(const char* hostname, int port)
{
	if (wsaResult != 0)
		throw WSASYSNOTREADY;

	tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpSocket == INVALID_SOCKET)
		throw WSAGetLastError();

	//set up address 
	sockaddr_in inetAddr;
	memset(&inetAddr, 0, sizeof(inetAddr));
	inetAddr.sin_port = htons(port);
	inetAddr.sin_family = AF_INET;
	InetPton(AF_INET, hostname, &inetAddr.sin_addr.s_addr);

	int rc = connect(tcpSocket, (SOCKADDR*)&inetAddr, sizeof(SOCKADDR));
	if (rc == SOCKET_ERROR) {
		throw WSAGetLastError();
	}
}

void TcpClient::Close()
{
#if WIN32
	closesocket(tcpSocket);
#else
	close(sock);
#endif
}

int TcpClient::Send(const uint8_t *Data, int Length) 
{
	int retCode = send(tcpSocket, (char*)Data, Length, 0);
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

int TcpClient::Receive(uint8_t *Data, int Length)
{	
	int retCode = recv(tcpSocket, (char*)Data, Length, 0);
	if (retCode == 0) {
		// connection has closed
		throw csModbusLib::ErrorCodes::CONNECTION_CLOSED;
	} else {
		if (retCode == SOCKET_ERROR) {
			int wsaError = WSAGetLastError();
			if (wsaError == WSAETIMEDOUT) {
				throw csModbusLib::ErrorCodes::RX_TIMEOUT;
			} else {
				DebugPrint ("WSA-error %d", wsaError);
				throw csModbusLib::ErrorCodes::CONNECTION_ERROR;
			}
		}
	}
	return retCode;
}

void TcpClient::SetReceiveTimeout(int timeout)
{
	DWORD rcvtimeo = timeout;
	setsockopt(tcpSocket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&rcvtimeo,
		sizeof(rcvtimeo));
}

