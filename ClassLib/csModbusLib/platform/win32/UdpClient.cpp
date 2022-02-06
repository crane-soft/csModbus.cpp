
// https://github.com/butcherg/DatagramSocket
// https://www.binarytides.com/udp-socket-programming-in-winsock/


#include "platform.h"
#include "MbSlave.h"
#include "UdpClient.h"

UdpClient::UdpClient()
{
	InitSocket();
}

UdpClient::UdpClient(const char* hostname, int port)
{
	InitSocket();
	Connect(hostname, port);
}

void UdpClient::InitSocket()
{
#ifdef WIN32
	WSADATA wsa;
	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
}

UdpClient::~UdpClient()
{
#if WIN32
	closesocket(udpSocket);
	WSACleanup();
#else
	close(sock);
#endif
}

void UdpClient::Connect(const char* hostname, int port)
{
	if (wsaResult != 0)
		throw WSASYSNOTREADY;

	udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpSocket == INVALID_SOCKET)
		throw WSAGetLastError();


	if (hostname != 0) {
		//set up address to use for sending
		memset(&outaddr, 0, sizeof(outaddr));
		outaddr.sin_port = htons(port);
		outaddr.sin_family = AF_INET;
		InetPton(AF_INET, hostname, &outaddr.sin_addr.s_addr);
	} else {
		//set up bind address
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(port);

		if (bind(udpSocket, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
			throw WSAGetLastError();
	}
}

void UdpClient::Close()
{
#if WIN32
	closesocket(udpSocket);
#else
	close(sock);
#endif
}

int UdpClient::Send(const uint8_t *Data, int Length) {
	return upd_sendto(Data, Length, &outaddr);
}

int UdpClient::SendResponse(const uint8_t *Data, int Length) {
	return upd_sendto(Data, Length, &remote_addr);
}

int UdpClient::upd_sendto(const uint8_t *Data, int Length, sockaddr_in *addr)
{
	int retCode = sendto(udpSocket, (char*)Data, Length, 0, (struct sockaddr *)addr, sizeof(sockaddr_in));
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

int UdpClient::Receive(uint8_t *Data, int Length)
{	
	socklen_t addrsize = sizeof(remote_addr);
	int retCode = recvfrom(udpSocket, (char*)Data, Length, 0, (struct sockaddr *)&remote_addr, &addrsize);
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

void UdpClient::SetReceiveTimeout(int timeout)
{
	DWORD rcvtimeo = timeout;
	setsockopt(udpSocket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&rcvtimeo,
		sizeof(rcvtimeo));
}


