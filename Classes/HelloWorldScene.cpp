#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "GameScene.h"
#include "SimpleAudioEngine.h"
#ifdef ANDROID
#include "android/log.h"
#endif

USING_NS_CC;


CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "exit.png",
                                        "exit_.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp( visibleSize.width/2 ,
                                 visibleSize.height/2-pCloseItem->getContentSize().height/2 -80));

	CCMenuItemImage *pStartItem = CCMenuItemImage::create(
		"start.png",
		"start_.png",
		this,
		menu_selector(HelloWorld::menuStartGameCallback));

	pStartItem->setPosition(ccp( visibleSize.width/2 ,
		visibleSize.height/2-pCloseItem->getContentSize().height/2 +80));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
	pMenu = CCMenu::create(pStartItem, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 2);
    /////////////////////////////
    // add "HelloWorld" splash screen"
	CCSprite* pSprite = CCSprite::create("logo_.png");
	pSprite->setPosition(ccp(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	// add the sprite as a child to this layer
	this->addChild(pSprite, 0);
	
    return true;
}


void HelloWorld::menuCloseCallback(CCObject* pSender)
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

void HelloWorld::menuStartGameCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
	CCScene* sc = GameScene::scene();             
	//Director::getInstance()->replaceScene(sc);                                                                   
	CCDirector::sharedDirector()->pushScene(sc); 
#endif
}