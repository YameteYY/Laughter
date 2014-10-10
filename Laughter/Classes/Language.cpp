#include "Language.h"
Language* Language::m_pInstance = 0;
void Language::Read()
{
	CCDictionary* plistDic = CCDictionary::createWithContentsOfFile("language.plist");
	download = *dynamic_cast<CCString*>(plistDic->objectForKey("download"));

	loading = *dynamic_cast<CCString*>(plistDic->objectForKey("loading"));

	ipAdrress = *dynamic_cast<CCString*>(plistDic->objectForKey("ip"));
}