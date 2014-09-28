#include "CSocket.h"
#pragma comment( lib, "ws2_32.lib"  )
CSocket::CSocket()
{
#if defined(WIN32)
	initSocket();
#endif
}
CSocket::~CSocket()
{
}
bool CSocket::connect(const char *adr, int port)
{
	this->m_szAddress = adr;
	this->m_nPort = port;
	sockaddr_in sa;
	struct hostent *hp;
	hp = gethostbyname(m_szAddress.c_str());
	if (hp)
	{
		const char *addr = inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]);
		struct in_addr ip;
		ip.s_addr = inet_addr(addr);
		sa.sin_family = AF_INET;
		sa.sin_port = htons(m_nPort);
		sa.sin_addr = ip;
	}
	if (sa.sin_port == 0)
	{
		return false;
	}
	int rs = 0;
	handlerSocket = socket(AF_INET, SOCK_STREAM, 0);
	rs =::connect(handlerSocket, (sockaddr *)&sa, sizeof(sa));
	return rs > -1;
}
int CSocket::read(char *buf, int bufLength)
{
	int readInt = recv(handlerSocket, buf, bufLength, 0);
	return readInt;
}
int CSocket::write(char *buf, int bufLength)
{
	int writeInt = send(handlerSocket, buf, bufLength, 0);
	return writeInt;
}
bool CSocket::close()
{
	int closeInt = 0;
#if defined(WIN32)
	closeInt =::shutdown(handlerSocket, 2);
#else
	closeInt =::shutdown(handlerSocket, SHUT_RDWR);
#endif
	if( closeInt > 0 )
	{
		return true;
	}
	return false;
}