#include "InputWidget.h"  
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
USING_NS_CC;

bool InputWidget::init()  
{  
	if (!CCLayer::init())    return false;  
	
	mSize=CCDirector::sharedDirector()->getWinSize();  
	mBg= CCSprite::create("msgbox.png");  
	mBg->setPosition(ccp(mSize.width / 2, mSize.height / 2));  
	this->addChild(mBg);  
	this->registerWithTouchDispatcher();

	//this->setTouchEnabled(true);
	return true;  
}  

void InputWidget::onOK(CCObject* pSender)  
{  
	this->removeFromParentAndCleanup(true);  
} 

bool InputWidget::onTouchBegan(Touch *pTouch, Event *pEvent)
{    
	//吞掉触摸事件  
	return true;    
}    

void InputWidget::onTouchMoved(Touch *pTouch, Event *pEvent)
{    

}    

void InputWidget::onTouchEnded(Touch *pTouch, Event *pEvent)
{    
	Menu* pMenu = (Menu*)this->getChildByTag(133);
	if ( pMenu && pMenu->ccTouchBegan(pTouch,pEvent))
		pMenu->ccTouchEnded(pTouch,pEvent);
}    

void InputWidget::onTouchCancelled(Touch *pTouch, Event *pEvent)
{    

}  

 //132 for text,133 for menu,134 for sprite
void InputWidget::SetShowType( int nFlag )
{
	using namespace CocosDenshion;
	if ( 0 == nFlag )//第一次提示你要打电话
	{
		CCLabelTTF* pLabel=CCLabelTTF::create("请找到电话并打给鑫林","Arial",40);
		pLabel->setColor(ccc3(0x80,0x4c,0x1b));
		pLabel->setPosition(ccp(mSize.width / 2, mSize.height / 2+40));  
		this->addChild(pLabel);  

		CCMenuItemImage *pMenuItem = CCMenuItemImage::create(  
			"ok_01.png",  
			"ok_02.png",  
			this,  
			menu_selector(InputWidget::onOK));  
		pMenuItem->setPosition(ccp(mSize.width / 2, mSize.height / 2-40));  
		CCMenu* pMenu = CCMenu::create(pMenuItem,NULL);  
		pMenu->setPosition(CCPointZero);  
		this->addChild(pMenu); 
		pMenu->setTag(133);
		SimpleAudioEngine::sharedEngine()->playEffect("alert.mp3");
	}
	else if ( 1 == nFlag )//显示正在拨通
	{
		CCSprite *ItemBox = CCSprite::create("phone.png");  

		ItemBox->setPosition(ccp(mSize.width / 2, mSize.height / 2));  
		ItemBox->setTag(134);		
		this->addChild(ItemBox); 
		SimpleAudioEngine::sharedEngine()->playEffect("phone.mp3");

		this->schedule(schedule_selector(InputWidget::step), 5.0f);
	}
	else if ( 2 == nFlag )//显示捡到钥匙
	{
		CCLabelTTF* pLabel=CCLabelTTF::create("你发现了一枚闪闪发亮的钥匙！","Arial",40);
		pLabel->setColor(ccc3(0x80,0x4c,0x1b));
		pLabel->setPosition(ccp(mSize.width / 2, mSize.height / 2+40));  
		this->addChild(pLabel);  
		SimpleAudioEngine::sharedEngine()->playEffect("ring.mp3");

		CCMenuItemImage *pMenuItem = CCMenuItemImage::create(  
			"ok_01.png",  
			"ok_02.png",  
			this,  
			menu_selector(InputWidget::onOK));  
		pMenuItem->setPosition(ccp(mSize.width / 2, mSize.height / 2-40));  
		CCMenu* pMenu = CCMenu::create(pMenuItem,NULL);  
		pMenu->setPosition(CCPointZero);  
		this->addChild(pMenu); 
		pMenu->setTag(133);
	}
	else if ( 3 == nFlag )//显示胜利
	{
		CCLabelTTF* pLabel=CCLabelTTF::create("恭喜逃出，快回家烧饭吧^.^","Arial",40);
		pLabel->setColor(ccc3(0x80,0x4c,0x1b));
		pLabel->setPosition(ccp(mSize.width / 2, mSize.height / 2+40));  
		this->addChild(pLabel);
		SimpleAudioEngine::sharedEngine()->playEffect("dooropen.mp3");
		//退出按钮
		CCMenuItemImage *pMenuItem = CCMenuItemImage::create(  
			"exit_.png",  
			"exit.png",  
			this,  
			menu_selector(InputWidget::onExit));  
		pMenuItem->setPosition(ccp(mSize.width / 2, mSize.height / 2-40));  
		CCMenu* pMenu = CCMenu::create(pMenuItem,NULL);  
		pMenu->setPosition(CCPointZero);  
		this->addChild(pMenu); 
		pMenu->setTag(133);
	}
}

void InputWidget::close()
{
	this->removeFromParentAndCleanup(true); 
}

//拨号中的延时
void InputWidget::step(float dt)
{
	this->unschedule(schedule_selector( InputWidget::step ));
	m_dispatcher->removeEventListener(m_listener);
	CCNode* pSprite = this->getChildByTag(134);
	CCTexture2D * texture =CCTextureCache::sharedTextureCache()->addImage("phone.png");
	//CCSpriteFrame * pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("phone_.png");  
	static_cast<CCSprite*>(pSprite)->setTexture(texture);

	using namespace CocosDenshion;
	SimpleAudioEngine::sharedEngine()->playEffect("wei.mp3") ;
						
	auto player = this->getParent()->getChildByTag(248);
	CCNode* pLabel = player->getChildByTag(250);
	static_cast<CCLabelTTF*>(pLabel)->setString("喂喂！");
}

void InputWidget::onExit(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#endif
}