#include "QiushiMgr.h"
#include "GB23122Utf8.h"
#include "cocos-ext.h"  
#include "../extensions/network/HttpClient.h"  
#include "../extensions/network/HttpRequest.h" 

USING_NS_CC;
USING_NS_CC_EXT;

QiushiMgr* QiushiMgr::m_pInstance = 0;
bool QiushiMgr::IsDown = true;

void QiushiMgr::ReadConfig()
{
	CCDictionary* plistDic = CCDictionary::createWithContentsOfFile("qiushi/config.plist");
	CCString* str = dynamic_cast<CCString*>(plistDic->objectForKey("currentPage"));
	mCurrentPage = str->intValue();
	
	str = dynamic_cast<CCString*>(plistDic->objectForKey("currentQiushi"));
	mCurrentQiushi = str->intValue();

	str = dynamic_cast<CCString*>(plistDic->objectForKey("largePage"));
	mLagePage = str->intValue();

	_readQiushiList(mCurrentPage);
	mCurrentQiushi--;
}
Qiushi* QiushiMgr::GetNextQiushi()
{
	if(mQiusiList.size() <= mCurrentQiushi + 1)
	{
		if(!_readQiushiList(++mCurrentPage))
			return 0;
		WriteConfig();
		mCurrentQiushi = 0;
	}
	mCurrentQiushi++;
	return &(mQiusiList[mCurrentQiushi]);
}
bool QiushiMgr::_readQiushiList(int page)
{
	mQiusiList.clear();
	char str[64];
	sprintf(str,"qiushi/%d.plist",page);
	if(!CCFileUtils::sharedFileUtils()->isFileExist(str))
		return false;
	CCDictionary* plistDic = CCDictionary::createWithContentsOfFile(str);
	int qiushiCount = plistDic->count();
	Qiushi temp;
	for(int i=0;i<qiushiCount;i++)
	{
		sprintf(str,"qiushi%d",i);
		CCDictionary* qiushiDic = dynamic_cast<CCDictionary*>(plistDic->objectForKey(str));
		temp.content = dynamic_cast<CCString*>(qiushiDic->objectForKey("content"))->m_sString;

		temp.picPath = dynamic_cast<CCString*>(qiushiDic->objectForKey("picPath"))->m_sString;
		mQiusiList.push_back(temp);
	}
	return true;
}
void QiushiMgr::DownLoadQiushi()
{
	IsDown = true;
	int errCode=0;
	do {
		pthread_attr_t tAttr;
		errCode=pthread_attr_init(&tAttr);
		CC_BREAK_IF(errCode!=0);
		errCode=pthread_attr_setdetachstate(&tAttr, PTHREAD_CREATE_DETACHED);
		if(errCode!=0)
		{
			pthread_attr_destroy(&tAttr);
			break;
		}
		errCode=pthread_create(&th_socket, &tAttr, DownLoad_thread, this);
	} while (0);
}
void QiushiMgr::UnDownLoadQiushi()
{
	IsDown = false;
}
void QiushiMgr::WriteConfig()
{
	char str[32];
	CCDictionary *dict = CCDictionary::create();
	sprintf(str,"%d",mCurrentPage);
	dict->setObject(CCString::create(str),"currentPage" );
	sprintf(str,"%d",mCurrentQiushi);
	dict->setObject(CCString::create(str),"currentQiushi");
	sprintf(str,"%d",mLagePage);
	dict->setObject(CCString::create(str),"largePage");
	dict->writeToFile("qiushi/config.plist");
}
void* QiushiMgr::DownLoad_thread(void *arg)
{
	CSocket* socket = QiushiMgr::Instance()->GetSocket();
	socket->connect("127.0.0.1",6000);
	char strLen[33];
	char* content;
	char picPath[64];
	vector<Qiushi> qiushiList;
	int qiushiCount = 0;
	int lagePage = QiushiMgr::Instance()->mLagePage;
	while(true){

		socket->write("haha",4);
		socket->read(strLen,32);
		int len = atoi(strLen);
		content = new char[len];
		socket->read(content,len+1);
		socket->read(strLen,32);
		
		len = atoi(strLen);
		picPath[0] = '\n';
		
		if(len != 0)
		{
			sprintf(picPath,"qiushi/pic/pic_%d_%d.jpg",lagePage,qiushiCount);
			char * buffer;
			buffer = new char [len];
			socket->write("haha",4);
			socket->read(buffer,len);
			FILE* file = fopen(picPath, "wb");
			
			fwrite(buffer,len,1,file);
			delete[] buffer;
			fclose(file);
		}
		qiushiList.push_back(Qiushi(content,picPath));
		delete[] content;
		qiushiCount++;
		if( qiushiCount == 100)
		{
			CCDictionary *dict = CCDictionary::create();
			dict->retain();
			for(int i=0;i<100;i++)
			{
				CCDictionary* qiushi = CCDictionary::create();
				qiushi->retain();
				qiushi->setObject(CCString::create(qiushiList[i].content),"content" );
				qiushi->setObject(CCString::create(qiushiList[i].picPath),"picPath" );
				sprintf(strLen,"qiushi%d",i);
				dict->setObject(qiushi,strLen);
			}
			sprintf(strLen,"qiushi/%d.plist",++lagePage);

			QiushiMgr::Instance()->mLagePage = lagePage;

			dict->writeToFile(strLen);
			dict->removeAllObjects();
			dict->release();
			qiushiList.clear();
			qiushiCount = 0;
		}
	}
	QiushiMgr::Instance()->WriteConfig();
	return NULL;
}