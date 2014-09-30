#include "LaughterScene.h"
#include "Language.h"

USING_NS_CC;
USING_NS_CC_EXT;

CCScene* LaughterScene::scene()
{
    CCScene *scene = CCScene::create();

    LaughterScene *layer = LaughterScene::create();

    scene->addChild(layer);

    return scene;
}
bool LaughterScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    QiushiMgr::Instance()->ReadConfig();
	Language::Instance()->Read();
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
	//this->addChild(mQiushiLabel);

	CCMenuItemImage *pNextItem = CCMenuItemImage::create(
		"next.png",
		"next.png",
		this,
		menu_selector(LaughterScene::menuNextCallback));
	pNextItem->setPosition(ccp(origin.x + visibleSize.width - pNextItem->getContentSize().width/2 ,
		origin.y + pNextItem->getContentSize().height/2));

	CCMenu* pMenu = CCMenu::create(pNextItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);
	CCScale9Sprite *psc9ButtonBG = CCScale9Sprite::create("download.png"); 

	mDownLoad = CCLabelTTF::create(Language::Instance()->download.getCString(), "Marker Felt", 16);
	CCControlButton *button = CCControlButton::create(mDownLoad,psc9ButtonBG);
	button->setPosition(ccp(origin.x + mDownLoad->getContentSize().width/2 + 10,
		origin.y + mDownLoad->getContentSize().height/2 + 5 ));
	this->addChild(button,1);
	button->addTargetWithActionForControlEvents(this,cccontrol_selector(LaughterScene::downLoadTouched),CCControlEventTouchUpInside);
	
	
	mScrollView = CCScrollView::create();
	mScrollView->setAnchorPoint(ccp(0,0));
	mScrollView->setBounceable(false);
	mScrollView->setDirection(kCCScrollViewDirectionVertical);  
	float he = origin.y + 30;
	mScrollView->setPosition(0,he);
	mScrollView->setViewSize(ccp(visibleSize.width + origin.x  ,visibleSize.height + origin.y - 30));
	mContainer = CCLayer::create();

	mQiushiLabel = CCLabelTTF::create("", "Marker Felt", 18);
	mQiushiLabel->setAnchorPoint(ccp(0,0));
	mQiushiLabel->setDimensions(ccp(visibleSize.width  ,0));
	ccColor3B colorText;
	colorText.r = 0;
	colorText.g = 0;
	colorText.b = 0;
	mQiushiLabel->setColor(colorText);
	mQiushiLabel->setHorizontalAlignment(kCCTextAlignmentLeft);
	mQiushiLabel->setVerticalAlignment(kCCVerticalTextAlignmentTop);
	mContainer->addChild(mQiushiLabel);
	mScrollView->setContainer(mContainer);
	this->addChild(mScrollView);

	Qiushi* content = QiushiMgr::Instance()->GetNextQiushi();
	mQiushiSprite = NULL;
	_displayQiushi(content);
	mIsDownLoad = false;

	schedule(schedule_selector(LaughterScene::_scheduleCheckDownLoad),1.0);
    return true;
}
void LaughterScene::_scheduleCheckDownLoad(float dt)
{
	QiushiMgr::Instance()->CheckDownLoadQiushi();
}
void LaughterScene::_displayQiushi(Qiushi* qiushi)
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	mQiushiLabel->setString(qiushi->content.c_str());
	float spriteHeight = 0;
	CCSize labelSize = mQiushiLabel->getContentSize();
	if(mQiushiSprite != NULL)
	{
		mContainer->removeChild(mQiushiSprite,true);
		mQiushiSprite = NULL;
	}
	if(qiushi->picPath != "")
	{
		mQiushiSprite = CCSprite::create(qiushi->picPath.c_str());
		mQiushiSprite->setAnchorPoint(ccp(0,0));
		CCSize spriteSize = mQiushiSprite->getContentSize();
		float scale = spriteSize.width/visibleSize.width;
		if(scale > spriteSize.height/visibleSize.height)
			scale = spriteSize.height/visibleSize.height;
		mQiushiSprite->setScale(scale);
		spriteHeight = spriteSize.height * scale;
		mQiushiSprite->setPosition(ccp(0,0));
		mContainer->addChild(mQiushiSprite);
	}
	mQiushiLabel->setPosition(ccp(origin.x,spriteHeight));
	float totalHieght = spriteHeight+labelSize.height;
	mScrollView->setContentSize(ccp(visibleSize.width + origin.x,totalHieght + origin.y));
	mScrollView->setContentOffset(ccp(0,-totalHieght));
}
void LaughterScene::menuNextCallback(CCObject* pSender)
{
	Qiushi* content = QiushiMgr::Instance()->GetNextQiushi();
	if(content != 0)
	{
		_displayQiushi(content);
	}
	else
	{
		mQiushiLabel->setString("No more ...\nPlease DownLoad");
		if(mQiushiSprite != NULL)
		{
			mContainer->removeChild(mQiushiSprite,true);
			mQiushiSprite = NULL;
		}
	}
}
void LaughterScene::downLoadTouched(CCObject* pSender,CCControlEvent eve)
{
	if(!mIsDownLoad)
	{
		QiushiMgr::Instance()->DownLoadQiushi();
		mDownLoad->setString(Language::Instance()->loading.getCString());
		mIsDownLoad = true;
	}
	else
	{
		mIsDownLoad = false;
		QiushiMgr::Instance()->UnDownLoadQiushi();
		mDownLoad->setString(Language::Instance()->download.getCString());
	}
}
