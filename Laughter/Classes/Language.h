#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "cocos2d.h"

using namespace cocos2d;
class Language
{
public:
	static Language* Instance()
	{
		if(m_pInstance == 0)
		{
			m_pInstance = new Language();
		}
		return m_pInstance;
	}
	CCString download;
	CCString loading;
	CCString ipAdrress;
	void Read();
private:
	Language(){}
	static Language* m_pInstance;
};

#endif