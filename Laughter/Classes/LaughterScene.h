#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__
#include "QiushiMgr.h"
#include "cocos2d.h"
#include "cocos-ext.h"  

class LaughterScene : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(LaughterScene);

	
private:
	void _scheduleCheckDownLoad(float dt);
	void menuNextCallback(CCObject* pSender);
	void downLoadTouched(CCObject* pSender,cocos2d::extension::CCControlEvent eve);
	void _displayQiushi(Qiushi* qiushi);
	cocos2d::CCSprite* mQiushiSprite;
	cocos2d::CCLabelTTF* mQiushiLabel;
	cocos2d::extension::CCScrollView* mScrollView;
	cocos2d::CCLayer* mContainer;
	cocos2d::CCLabelTTF* mDownLoad;
	bool mIsDownLoad;
};

#endif // __HELLOWORLD_SCENE_H__
