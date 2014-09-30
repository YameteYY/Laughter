#ifndef __CONTENTMGR_H__
#define __CONTENTMGR_H__
#include "cocos2d.h"
#include "CSocket.h"
#include <pthread.h>
#include <string>
#include <vector>

using namespace std;
struct Qiushi
{
	Qiushi(string c,string p)
	{
		content = "    " + c;
		picPath = p;
	}
	Qiushi(){}
	string content;
	string picPath;
};
class QiushiMgr
{
public:
	static QiushiMgr* Instance()
	{
		if(m_pInstance == 0)
		{
			m_pInstance = new QiushiMgr();
		}
		return m_pInstance;
	}
	Qiushi* GetNextQiushi();
	void ReadConfig();
	void WriteConfig();
	void DownLoadQiushi();
	void UnDownLoadQiushi();
	void CheckDownLoadQiushi();
	CSocket* GetSocket();
private:
	CSocket mSocket;
	static bool IsDown;
	static void* DownLoad_thread(void *arg);// 被启动的线程函数，注意必须是静态方法
	pthread_t th_socket; // 起这个名字本打算用在socket上的
	bool _readQiushiList(int page);
	int mCurrentPage;
	int mLagePage;
	int mCurrentQiushi;
	int mPicIndex;
	vector<Qiushi> mQiusiList;
	vector<Qiushi> mDownQiushiList;
	static QiushiMgr* m_pInstance;
	QiushiMgr(){}
	
};
inline CSocket* QiushiMgr::GetSocket()
{
	return &mSocket;
}

#endif