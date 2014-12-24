#include "InputWidget.h"
#include "GameScene.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
#include <math.h>

USING_NS_CC;
   
CCScene* GameScene::scene()
{
	// 'scene' is an autorelease object
	CCScene *scene = CCScene::create();

	// 'layer' is an autorelease object
	GameScene *layer = GameScene::create();
    layer->setTag(248);
	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}
const int ID_TEXT=251;
bool GameScene::init()  
{  
	if ( !CCLayer::init() )  return false;  


	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();  
	CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();  
	//背景
	CCSprite *bg = CCSprite::create("game1.png");  
	bg->setPosition(CCPoint(origin.x + visibleSize.width/2,origin.y+visibleSize.height /2));  
	this->addChild(bg);
	sBg = bg;
	//道具栏
	{
	CCSprite *ItemBox = CCSprite::create("itembox.png");  
	ItemBox->setPosition(CCPoint(origin.x + visibleSize.width - ItemBox->getContentSize().width/2-30,  
		origin.y +visibleSize.height - ItemBox->getContentSize().height/2 - 30));  
	this->addChild(ItemBox); 
	}
	//会议室标记
	{
	CCSprite *office = CCSprite::create("station.png");  
	office->setPosition(CCPoint(origin.x + visibleSize.width/2  ,  
		origin.y +visibleSize.height - office->getContentSize().height/2 - 30));  
	this->addChild(office); 
	}
	 
	CCMenuItemImage *pTalk = CCMenuItemImage::create(
		"talk.png",
		"talk.png",
		this,
		menu_selector(GameScene::menuTalkCallback));
	 
	pTalk->setScaleX( visibleSize.width / pTalk->getContentSize().width * 2);
	pTalk->setPosition(ccp( 0, 0+65));
	//设置标签纹理的字体和大小，纹理尺寸和横向对齐方式为剧中对齐  
	// create menu, it's an autorelease object
	CCMenu* pMenu = CCMenu::create(pTalk, NULL);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu);

	LabelTTF *pLabel = LabelTTF::create(".....(点击黑色区域继续)","Arial",24/2,CCSizeMake(1900,100),kCCTextAlignmentLeft);
	pLabel->setTag(ID_TEXT);
	// position the label on the center of the screen
	pLabel->setPosition(ccp(origin.x + 950 +40,origin.y + 0+ 30));
	this->addChild(pLabel);
	
  	m_TextProgress = 0;
	SwitchBg(1);	

	CCSize bgSize = sBg->getContentSize() ;
	m_scale = 0.5f;  
	sBg->setScale(m_scale);
	bgOrigin = ccp(origin.x + visibleSize.width/2 - bgSize.width/2 * m_scale , origin.y + visibleSize.height/2 - bgSize.height/2 * m_scale);  

	CCLOG( "bgOrigin:%1f,%1f" , bgOrigin.x,bgOrigin.y  );
	//初始化数据  

	distance = 0.0f;  
	_touchesDic = CCDictionary::create();  
	_touchesDic->retain();  

	m_HasKey = false;
	m_HasOpened = false;
	m_bCanCLickTalk =true;
	m_HasCalled = false;
	m_rectDoor = CCRectMake(196+20,2592-841-112,72,112);
	m_rectKey = CCRectMake(1550,2592-1101-30 ,40,36);
	m_rectPhone = CCRectMake(1600,2592-1607-73,172,73);

	this->setTouchEnabled( true);
	return true;  
}  

void GameScene::menuOkCallback(CCObject* pSender)                                                                               
{  
	CCDirector::sharedDirector()->popScene();                                                                                           
}  
void GameScene::menuSoundToggleCallback(CCObject* pSender)  
{

}  
void GameScene::menuMusicToggleCallback(CCObject* pSender)  
{

}  

void GameScene::menuTalkCallback( CCObject* pSender )
{
	if (!m_bCanCLickTalk)
		  return;
	
	 m_TextProgress ++;
	 CCLabelTTF *pLabel = (CCLabelTTF*)this->getChildByTag(ID_TEXT);
	 using namespace CocosDenshion;
	 switch(m_TextProgress)
	 {
	 case 1:
		 pLabel->setString("...");
		 break;
	 case 2:
		 pLabel->setString("唉？开会又睡着了吗？");
		 break;
	 case 3:
		 pLabel->setString("呀，真没办法，一开会就困。");
		 break;
	 case 4:
		 pLabel->setString("（咔。咔。）");
		 SimpleAudioEngine::getInstance()->playEffect("door.mp3"); 
		 break;
	 case 5:
		 pLabel->setString("什么？把门锁了？没见我在这睡觉吗？");
		 break;
	 case 6:
		 pLabel->setString("时间是？六点，一刻");
		 break;
	 case 7:
		 pLabel->setString("！！！");
		 break;
	 case 8:
		 pLabel->setString("完蛋了！！！还没回去给老婆烧饭！");
		 break;
	 case 9:
		 pLabel->setString("回家一定要跪搓键盘了...");	 
		 break;
	 case 10:
		 pLabel->setString("(咕咕咕。。。)");	
		 break;
	 case 11:
		 pLabel->setString("好饿，好冷，怎么办？难道我要饿死在这里了？");
		 break;
	 case 12:
		 pLabel->setString("谁要买火柴啊？一块钱一盒的火柴，一块钱你买不了吃亏");
		 break;
	 case 13:
		 {
		 pLabel->setString("还是打个电话找谁来救救我吧");
		 CCLayer* sc = InputWidget::create();
		 static_cast<InputWidget*>(sc)->SetShowType(0);
		 this->getParent()->addChild(sc);  
		 m_bCanCLickTalk = false;
		 }
		 break;
	 case 14:
		 {
 		 pLabel->setString("(正在呼叫。。。)");
		 CCLayer* sc = InputWidget::create();
		 static_cast<InputWidget*>(sc)->SetShowType(1);
		 sc->setTag(249);
		 this->getParent()->addChild(sc); 
		 }
		 break;
	 case 15:
		 pLabel->setString("鑫林，是你吗，我被反锁在会议室，快来帮帮我！");
		break;
	 case 16:
		 pLabel->setString("李：唉，就知道你出不去");
		 break;
	 case 17:
		 pLabel->setString("李：你开会睡那么香，踹都踹不醒");
		 break;
	 case 18:
		 pLabel->setString("好了，别说那么多，快救我出去");
		 break;
	 case 19:
		 pLabel->setString("李：安心安心，钥匙我给你留了，在花盆里");
		 break;
	 case 20:
		 pLabel->setString("哪个花盆啊？");
		 break;
	 case 21:
		 {
		 pLabel->setString("擦叻！手机这时候没电，让你丫开会玩手机");
		 CCNode* sc = this->getParent()->getChildByTag(249);
		 static_cast<InputWidget*>(sc)->close();
		 m_HasCalled =true;
		 }
	     break;
	 case 22:
		 pLabel->setString("好吧，我自己找，反正没几个花盆");
		 break;
	 case 23:
		pLabel->setString(" ");
		break;
	 }
}

void GameScene::onTouchesBegan( const std::vector<Touch*>& touches, Event *unused_event  )
{
	
	for (auto iter = touches.begin() ; iter != touches.end(); iter++){  
		CCTouch* pTouch = (CCTouch*)(*iter);  
			_touchesDic->setObject(pTouch, CCString::createWithFormat("%d",pTouch->getID())->getCString());  
	}  

	//两个手指  
	if (_touchesDic->count() == 2){  
		CCArray* keys = _touchesDic->allKeys();  
		CCTouch *touch1 = (CCTouch*)_touchesDic->objectForKey(((CCString*)keys->objectAtIndex(0))->getCString());  
		CCTouch *touch2 = (CCTouch*)_touchesDic->objectForKey(((CCString*)keys->objectAtIndex(1))->getCString());  

		CCPoint pt = touch1->getLocation();  
		CCPoint pt2 = touch2->getLocation();  

		distance = ccpDistance(pt,pt2);  
	} 
	if (_touchesDic->count() == 1)//拾取物品
	{
		CCArray* keys = _touchesDic->allKeys();  
		CCTouch *touch1 = (CCTouch*)_touchesDic->objectForKey(((CCString*)keys->objectAtIndex(0))->getCString());
		CCPoint pt = touch1->getLocation(); 
		pt = CalcRealPosition(pt);
		if (m_rectPhone.containsPoint(pt))//点击手机
		{
			m_bCanCLickTalk =true;
			menuTalkCallback(NULL);
		}
		else if (m_HasCalled && !m_HasKey &&  m_rectKey.containsPoint(pt))	//已打电话未拿钥匙
		{
			m_HasKey = true;
			CCSprite *Item = CCSprite::create("key.png"); 
			CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();  
			CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin(); 
			Item->setPosition(CCPoint(origin.x + visibleSize.width - Item->getContentSize().width/2 - 30,  
				origin.y +visibleSize.height - Item->getContentSize().height/2 - 30));  
			this->addChild(Item); 

			//弹出提示
			CCLayer* sc = InputWidget::create();
			static_cast<InputWidget*>(sc)->SetShowType(2);
			sc->setTag(249);
			this->getParent()->addChild(sc); 
			//换背景
			SwitchBg(2);
			//下面增加提示
			CCLabelTTF *pLabel = (CCLabelTTF*)this->getChildByTag(ID_TEXT);
			pLabel->setString("拿到钥匙了，快去开门出去吧！");
		}
		else if (m_HasKey && !m_HasOpened && m_rectDoor.containsPoint(pt))
		{
			CCLayer* sc = InputWidget::create();
			static_cast<InputWidget*>(sc)->SetShowType(3);
			sc->setTag(249);
			this->getParent()->addChild(sc); 
			m_HasOpened = true;

			SwitchBg(3);
		}
	}
}

void GameScene::onTouchesMoved( const std::vector<Touch*>& touches, Event *unused_event  )
{
	CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();  
	if (_touchesDic->count() == 2) 
	{  
		CCArray* keys = _touchesDic->allKeys();  
		CCTouch *touch1 = (CCTouch*)_touchesDic->objectForKey(((CCString*)keys->objectAtIndex(0))->getCString());  
		CCTouch *touch2 = (CCTouch*)_touchesDic->objectForKey(((CCString*)keys->objectAtIndex(1))->getCString());  

		CCPoint point1 = touch1->getLocation();  
		CCPoint point2 = touch2->getLocation();  

		//点击点距原点距离  
		CCPoint mPoint1 = ccpSub(point1,bgOrigin);  
		CCPoint mPoint2 = ccpSub(point2,bgOrigin);    
		//点击点的中心点  
		float x = (mPoint2.x+mPoint1.x)/2 ;    
		float y = (mPoint2.y+mPoint1.y)/2 ;  

		//得到锚点值   
		CCSize bgSize = m_bgSize;  
		float anchorX = x/(bgSize.width*sBg->getScale());  
		float anchorY = y/(bgSize.height*sBg->getScale());  
		sBg->setAnchorPoint(ccp(anchorX,anchorY));  

		//位置  
		x = (point2.x+point1.x)/2 ;    
		y = (point2.y+point1.y)/2 ;   
		sBg->setPosition(ccp(x ,y));  

		//放大的倍率  
		float mdistance = ccpDistance(mPoint1,mPoint2);  
		m_scale = (mdistance/distance)*m_scale;  
		if (m_scale>=9.0)//放大缩小的最值  
			sBg->setScale(9.0f);  
		else if (m_scale<=0.5)  
			sBg->setScale(0.5f);  
		else  
		{  
			distance = mdistance;  
			sBg->setScale(m_scale);  
		}  
 
		m_scale = sBg->getScale();  

		//原点值  
		bgOrigin  = ccpSub(ccp(x,y),ccp(bgSize.width*sBg->getScale()*anchorX,bgSize.height*sBg->getScale()*anchorY));  
	}  
	else if (_touchesDic->count() == 1)//单指  
	{  
		auto iter = touches.begin();    
		Point pDelta = (*iter)->getDelta();  
		CCPoint postion = sBg->getPosition();  
		
		CCPoint checkPoint = postion /m_scale; 	 

		if ( checkPoint.x <0  && pDelta.x < 0 )	//防止超边
			pDelta.x = 0;
		if ( checkPoint.x >  sBg->getContentSize().width/2  && pDelta.x > 0 )
			pDelta.x = 0;
		if ( checkPoint.y < 0  && pDelta.y < 0 )
			pDelta.y = 0;
		if ( checkPoint.y >  sBg->getContentSize().height/2 && pDelta.y > 0 )
			pDelta.y = 0;

		//原点位置  
		sBg->setPosition( ccpAdd(postion,pDelta) );  
		bgOrigin = ccpAdd(bgOrigin,pDelta);  				
	}  
}

void GameScene::onTouchesEnded( const std::vector<Touch*>& touches, Event *unused_event )
{
	if (_touchesDic->count() == 2)
		distance = 0.0f;  

	for (auto iter = touches.begin(); iter != touches.end(); iter++){  
		CCTouch* pTouch = (CCTouch*)(*iter);  
		_touchesDic->removeObjectForKey(CCString::createWithFormat("%d",pTouch->getID())->getCString());  
	} 
}


CCPoint GameScene::CalcRealPosition(CCPoint touchPoint)
{

	CCPoint realPoint = touchPoint / m_scale - bgOrigin/m_scale;
	return realPoint;
}


void GameScene::SwitchBg(int nIndex)
{
	int w=480;
	int h=320;


	CCSize sizeImg(480,320);

	if (nIndex == 1)
	{
		CCTexture2D * texture = CCTextureCache::sharedTextureCache()->addImage("game1.png");
		//CCSpriteFrame * pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("phone_.png");  
		static_cast<CCSprite*>(sBg)->setTexture(texture);
		m_bgSize = sBg->getContentSize();
	}
	else if (nIndex == 2)
	{
		CCTexture2D * texture = CCTextureCache::sharedTextureCache()->addImage("game2.png");
		//CCSpriteFrame * pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("phone_.png");  
		static_cast<CCSprite*>(sBg)->setTexture(texture);
		m_bgSize = sBg->getContentSize();
	}
	else if (nIndex ==3 )
	{
		CCTexture2D * texture = CCTextureCache::sharedTextureCache()->addImage("game3.png");
		//CCSpriteFrame * pFrame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("phone_.png");  
		static_cast<CCSprite*>(sBg)->setTexture(texture);
		m_bgSize = sBg->getContentSize();
	} 
}