#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include "cocos2d.h"

USING_NS_CC;
class GameScene : public cocos2d::CCLayer
{
public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();  

	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::CCScene* scene();

	// implement the "static node()" method manually
	CREATE_FUNC(GameScene);
		// a selector callback
	void menuOkCallback(CCObject* pSender);
	void menuTalkCallback(CCObject* pSender);
	void menuSoundToggleCallback(CCObject* pSender);
	void menuMusicToggleCallback(CCObject* pSender);
	//����¼�  
	virtual void onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event);
	virtual void onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event);
	virtual void onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event);
	CCPoint CalcRealPosition(CCPoint touchPoint);
	void SwitchBg(int nIndex);

private:
	cocos2d::CCSprite * sBg;
	CCSize m_bgSize;
	float distance;   //��ֻ��ָ���ʱ�ĳ�ʼ����  
	float m_scale; 	//�Ŵ�ı���  
	CCPoint bgOrigin;  	//��¼ԭ��λ�� 
	__Dictionary* _touchesDic;  
	//��touch�¼�  
	int m_TextProgress;
	int m_Scales;
	int m_XOffset;
	int m_YOffset;

	CCRect m_rectKey;
	CCRect m_rectPhone;
	CCRect m_rectDoor;

	bool m_bCanCLickTalk;
	bool m_HasCalled;
	bool m_HasKey;
	bool m_HasOpened;
};

#endif
