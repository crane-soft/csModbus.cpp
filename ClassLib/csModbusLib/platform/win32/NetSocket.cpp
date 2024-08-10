#include "NetSocket.h"

NetSocket::NetSocket()
{
#ifdef WIN32
	WSADATA wsa;
	wsaResult = WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

}

NetSocket::~NetSocket()
{
#if WIN32
	closesocket(Socket);
	WSACleanup();
#else
	close(sock);
#endif

}

void NetSocket::SetReceiveTimeout(int timeout)
{
	DWORD rcvtimeo = timeout;
	setsockopt(Socket,
		SOL_SOCKET,
		SO_RCVTIMEO,
		(const char *)&rcvtimeo,
		sizeof(rcvtimeo));
}
