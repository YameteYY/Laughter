#ifndef __CSocket__
#define __CSocket__
#if CC_TARGET_PLATFORM==CC_PLATFORM_WIN32
#include <winsock.h>
#elif CC_TARGET_PLATFORM==CC_PLATFORM_ANDROID
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#elif CC_TARGET_PLATFORM==CC_PLATFORM_IOS
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <string>
using namespace std;
class CSocket{
public:
	CSocket();
	~CSocket();
	bool connect(const char* _adr,int _port);
	int read(char *buf,int size);
	int write(char *buf,int size);
	bool close();
private:
	string m_szAddress;
	int m_nPort;
#if CC_TARGET_PLATFORM==CC_PLATFORM_WIN32
	SOCKET handlerSocket;
	//����������ֻ����windows�������
	//��ʼ������ģ�飬һ������������main�����������һ�ξͿ����ˡ�
	void initSocket()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int ret;
		//WinSock��ʼ��
		wVersionRequested = MAKEWORD(1, 1); //ϣ��ʹ�õ�WinSock DLL�İ汾
		ret = WSAStartup(wVersionRequested, &wsaData);
		if(ret!=0)
		{
			printf("WSAStartup() failed!\n");
			return;
		}
		//ȷ��WinSock DLL֧�ְ汾2.2
		if(LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
		{
			WSACleanup();
			printf("Invalid WinSock version!\n");
			return;
		}
	}
	//�������Ϸ�˳���ʱ����þͿ�����
	void uninitSocket()
	{
		WSACleanup(); 
	}
#else
	int handlerSocket;
#endif
};
#endif