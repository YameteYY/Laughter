#include <stdio.h>
#include <iostream>
#include <string>
#include <Winsock2.h>
#include <fstream>
#include "mysql.h"
using namespace std;
#pragma comment( lib, "ws2_32.lib"  )
#pragma comment( lib, "libmysql.lib")

const char* gb23122utf8(const char* gb2312)    
{  
	int len = MultiByteToWideChar(0, 0, gb2312, -1, NULL, 0);    
	wchar_t* wstr = new wchar_t[len+1];    
	memset(wstr, 0, len+1);    
	MultiByteToWideChar(0, 0, gb2312, -1, wstr, len);    
	len = WideCharToMultiByte(65001, 0, wstr, -1, NULL, 0, NULL, NULL);    
	char* str = new char[len+1];
	memset(str, 0, len+1);    
	WideCharToMultiByte(65001, 0, wstr, -1, str, len, NULL, NULL);    
	if(wstr) delete[] wstr;    
	return str;
} 
int main()
{
	const char user[] = "root";         //username
	const char pswd[] = "345920";         //password
	const char host[] = "127.0.0.1";    //or"127.0.0.1"
	const char table[] = "laughterdb";        //database
	unsigned int port = 3306;           //server port    
	MYSQL myCont;
	MYSQL_RES *result;
	MYSQL_ROW sql_row;
	MYSQL_FIELD *fd;
	char column[32][32];
	int res;
	mysql_init(&myCont);
	if(!mysql_real_connect(&myCont,host,user,pswd,table,port,NULL,0))
	{
		const char * s = mysql_error(&myCont);
		std::cout << s<<std::endl;
		return 0;
	}
	std::string SQL("SELECT content,picPath FROM qiushi where id_order = '");
	mysql_set_character_set(&myCont,"gbk");

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 1, 1 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		return 0;
	}

	if ( LOBYTE( wsaData.wVersion ) != 1 ||
		HIBYTE( wsaData.wVersion ) != 1 ) {
			WSACleanup( );
			return 0;
	}
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(6000);

	bind(sockSrv,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	listen(sockSrv,5);
	SOCKADDR_IN addrClient;
	int len=sizeof(SOCKADDR);
	SOCKET sockConn=accept(sockSrv,(SOCKADDR*)&addrClient,&len);
	while(1)
	{
		char recvBuf[16];
		recv(sockConn,recvBuf,16,0);
		int a = 1 + rand()%100;
		sprintf(recvBuf,"%d",a);
		std::string qul = SQL + recvBuf;
		qul += "'";
		mysql_query(&myCont,qul.c_str());
		MYSQL_RES *m_res = mysql_store_result(&myCont);

		std::string str("");
		MYSQL_ROW m_row;
		int rnum = 0;
		m_row = mysql_fetch_row(m_res);
		str = m_row[0];
		const char* tt = gb23122utf8(str.c_str());
		
		int strLen = strlen(tt);
		char strLenStr[32];
		sprintf(strLenStr,"%d",strLen);
		
		send(sockConn,strLenStr,32 ,0);
		puts(strLenStr);

		send(sockConn,tt,strLen+1,0);
		

		str = m_row[1];
		str = "";
		if(str != "")
		{
			char * buffer;
			int size;
			ifstream file (str.c_str(), ios::in|ios::binary|ios::ate);
			size = file.tellg();
			file.seekg (0, ios::beg);
			buffer = new char [size];
			file.read (buffer, size);
			file.close();
			sprintf(strLenStr,"%d",size);
			puts(strLenStr);
			send(sockConn,strLenStr,32 ,0);
			recv(sockConn,recvBuf,16,0);
			send(sockConn,buffer,size,0);
		}
		else
		{
			send(sockConn,"0",2,0);
		}
	}
	return 0;
}