/****************************************************************************
Copyright (c) 2010-2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "AppDelegate.h"

#include <vector>
#include <string>

#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
using namespace std;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto pDirector = Director::getInstance();
	auto glview = pDirector->getOpenGLView();
	if(!glview) {
		glview = GLView::create("My Game");
		pDirector->setOpenGLView(glview);
	}

	// turn on display FPS
	pDirector->setDisplayStats(true);
	pDirector->setAnimationInterval(1.0 / 60);

	glview->setDesignResolutionSize(designResolutionSize.width,designResolutionSize.height, kResolutionNoBorder);

	CCSize frameSize = glview->getFrameSize();
	vector<string> searchPath;

	if (frameSize.height > mediumResource.size.height)
	{
	   // searchPath.push_back("xiaomi");
	    pDirector->setContentScaleFactor(MIN(largeResource.size.height/designResolutionSize.height, largeResource.size.width/designResolutionSize.width));
	}
	else if (frameSize.height > smallResource.size.height)
	{
	   // searchPath.push_back("xiaomi");
	    pDirector->setContentScaleFactor(MIN(mediumResource.size.height/designResolutionSize.height, mediumResource.size.width/designResolutionSize.width));
	}
	else																   
	{
	   // searchPath.push_back("xiaomi");
	    pDirector->setContentScaleFactor(MIN(smallResource.size.height/designResolutionSize.height, smallResource.size.width/designResolutionSize.width));
	}
	searchPath.push_back("xiaomiHD");
	CCFileUtils::getInstance()->setSearchPaths(searchPath);

    // create a scene. it's an autorelease object
    CCScene *pScene = HelloWorld::scene();
    pDirector->runWithScene(pScene);

	using namespace CocosDenshion;
	// preload background music and effect  
	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic( "Loop.wav" );  
	//CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect( "" );  

	// set default volume  
	SimpleAudioEngine::sharedEngine()->setEffectsVolume(0.6);  
	SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.3);  
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("Loop.wav", true);  

	SimpleAudioEngine::sharedEngine()->preloadEffect("alert.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("door.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("ring.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("dooropen.mp3");
	SimpleAudioEngine::sharedEngine()->preloadEffect("wei.mp3");

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    CCDirector::sharedDirector()->stopAnimation();
	CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();

    // if you use SimpleAudioEngine, it must be pause
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    CCDirector::sharedDirector()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}
