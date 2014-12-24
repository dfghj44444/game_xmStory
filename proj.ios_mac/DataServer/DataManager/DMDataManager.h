//
//  DMDataManager.h
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __DataManager__DMDataManager__
#define __DataManager__DMDataManager__

#include <stdio.h>
#include <string>

class DMDataManagerCallback;

class DMDataManager {
public:
    DMDataManager();
    virtual ~DMDataManager() = 0;
    
    virtual void setImageId(int id) = 0;
    virtual void setScreenSize(int width, int height) = 0;
    
    virtual void downloadImage() = 0;
    
    virtual void requestImageInfo() = 0;
    
    virtual void passScaleInfo(int x,int y,float scale) = 0; // x,y中心点坐标
    
    virtual void setDataCallback(DMDataManagerCallback *callback) = 0;
    
    virtual void saveImageDataToLocation(unsigned char *data, unsigned long dataLength) = 0;
    
    static DMDataManager *CreateDataManager();
    static void ReleaseDataManager(DMDataManager *dataManager);
};

#endif /* defined(__DataManager__DMDataManager__) */
