#include "QiushiMgr.h"
#include "GB23122Utf8.h"
#include "Language.h"
#include "cocos-ext.h"  
#include "../extensions/network/HttpClient.h"  
#include "../extensions/network/HttpRequest.h" 

USING_NS_CC;
USING_NS_CC_EXT;

pthread_mutex_t  counter_mutex = PTHREAD_MUTEX_INITIALIZER;

QiushiMgr* QiushiMgr::m_pInstance = 0;
bool QiushiMgr::IsDown = true;

void QiushiMgr::ReadConfig()
{
	CCDictionary* plistDic = CCDictionary::createWithContentsOfFile("config.plist");
	CCString* str = dynamic_cast<CCString*>(plistDic->objectForKey("currentPage"));
	mCurrentPage = str->intValue();
	
	str = dynamic_cast<CCString*>(plistDic->objectForKey("currentQiushi"));
	mCurrentQiushi = str->intValue();

	str = dynamic_cast<CCString*>(plistDic->objectForKey("largePage"));
	mLagePage = str->intValue();

	str = dynamic_cast<CCString*>(plistDic->objectForKey("picIndex"));
	mPicIndex = str->intValue();

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
	DownLoad_thread(NULL);
	return;
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
void QiushiMgr::CheckDownLoadQiushi()
{
	pthread_mutex_lock(&counter_mutex);
	int len = mDownQiushiList.size();
	if(  len >= 100)
	{
		char strLen[64];
		CCDictionary *dict = CCDictionary::create();
		dict->retain();
		for(int i=0;i<len;i++)
		{
			CCDictionary* qiushi = CCDictionary::create();
			qiushi->retain();
			CCString* conCCS = CCString::create(mDownQiushiList[i].content);
			conCCS->retain();
			qiushi->setObject(conCCS,"content" );
			CCString* picCCS = CCString::create(mDownQiushiList[i].picPath);
			picCCS->retain();
			qiushi->setObject(picCCS,"picPath" );
			sprintf(strLen,"qiushi%d",i);

			dict->setObject(qiushi,strLen);
			conCCS->release();
			picCCS->release();
		}
		sprintf(strLen,"qiushi/%d.plist",++mLagePage);

		dict->writeToFile(strLen);
		dict->removeAllObjects();
		dict->release();

		mDownQiushiList.clear();
	}
	pthread_mutex_unlock(&counter_mutex);
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
	sprintf(str,"%d",mPicIndex);
	dict->setObject(CCString::create(str),"picIndex");
	dict->writeToFile("config.plist");
}
void* QiushiMgr::DownLoad_thread(void *arg)
{
	CSocket* socket = QiushiMgr::Instance()->GetSocket();
	CCLOG("pass:%s",Language::Instance()->ipAdrress.getCString());
	if(!socket->connect(Language::Instance()->ipAdrress.getCString(),1234))
		return NULL;
	CCLOG("connect");
	char strLen[33];
	char picPath[64];

	pthread_mutex_lock(&counter_mutex);
	vector<Qiushi>* qiushiList = &(QiushiMgr::Instance()->mDownQiushiList);
	int mPicIndex = QiushiMgr::Instance()->mPicIndex;
	pthread_mutex_unlock(&counter_mutex);
	int qiushiCount = 0;
	while(IsDown){
		socket->write("haha",4);
		socket->read(strLen,32);
		int len = atoi(strLen);
		char * content = new char [len+1];
		socket->read(content,len+1);
		socket->read(strLen,32);
		int picLen = atoi(strLen);
		sprintf(picPath,"");
		if(picLen != 0)
		{
			sprintf(picPath,"qiushi/pic/pic_%d.jpg",++mPicIndex);
			char * buffer;
			buffer = new char [picLen];
			socket->write("haha",4);
			int cc = socket->read(buffer,picLen);
			FILE* file = fopen(picPath, "wb");
			fwrite(buffer,picLen,1,file);
			delete[] buffer;
			fclose(file);
		}
		pthread_mutex_lock(&counter_mutex);
		(*qiushiList).push_back(Qiushi(content,picPath));
		QiushiMgr::Instance()->mPicIndex = mPicIndex;
		pthread_mutex_unlock(&counter_mutex);
		delete[] content;
		/*
		qiushiCount++;
		if( qiushiCount == 100)
		{
			CCDictionary *dict = CCDictionary::create();
			dict->retain();
			for(int i=0;i<100;i++)
			{
				CCDictionary* qiushi = CCDictionary::create();
				qiushi->retain();
				CCString* conCCS = CCString::create(qiushiList[i].content);
				conCCS->retain();
				qiushi->setObject(conCCS,"content" );
				CCString* picCCS = CCString::create(qiushiList[i].picPath);
				picCCS->retain();
				qiushi->setObject(picCCS,"picPath" );
				sprintf(strLen,"qiushi%d",i);

				dict->setObject(qiushi,strLen);
				conCCS->release();
				picCCS->release();
			}
			sprintf(strLen,"qiushi/%d.plist",++lagePage);

			QiushiMgr::Instance()->mLagePage = lagePage;

			dict->writeToFile(strLen);
			dict->removeAllObjects();
			dict->release();
			
			qiushiList.clear();
			qiushiCount = 0;
		}*/
	}
	return NULL;
}