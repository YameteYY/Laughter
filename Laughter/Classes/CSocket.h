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
	//这两个函数只有在windows下面才用
	//初始化网络模块，一定不能忘，在main函数里面调用一次就可以了。
	void initSocket()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int ret;
		//WinSock初始化
		wVersionRequested = MAKEWORD(1, 1); //希望使用的WinSock DLL的版本
		ret = WSAStartup(wVersionRequested, &wsaData);
		if(ret!=0)
		{
			printf("WSAStartup() failed!\n");
			return;
		}
		//确认WinSock DLL支持版本2.2
		if(LOBYTE(wsaData.wVersion)!=1 || HIBYTE(wsaData.wVersion)!=1)
		{
			WSACleanup();
			printf("Invalid WinSock version!\n");
			return;
		}
	}
	//这个在游戏退出的时候调用就可以了
	void uninitSocket()
	{
		WSACleanup(); 
	}
#else
	int handlerSocket;
#endif
};
#endif