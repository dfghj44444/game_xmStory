//
//  DMImageManager.h
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __DataManager__DMImageManager__
#define __DataManager__DMImageManager__

#include "DMDataManager.h"
#include "DMDataManagerCallback.h"
#include "DMBlockDataCache.h"
#include <vector>

#include "TaskQueue.h"

class BlockInfo;
class ScaleInfo;
class DMImageRequestManager;

class DMImageManager : public DMDataManager, public DMDataManagerCallback, public TaskCallback {
public:
    DMImageManager();
    virtual ~DMImageManager();
    
    virtual void setImageId(int id);
    virtual void setScreenSize(int width, int height);
    
    virtual void downloadImage();
    
    virtual void requestImageInfo();
    
    virtual void passScaleInfo(int x,int y,float scale);
    
    virtual void setDataCallback(DMDataManagerCallback *callback);
    
    virtual void didFinishGetImageInfo(DMImageInfo *imageInfo);
    
    virtual void didFinishGetImageTileData(int imageId, unsigned char *data, unsigned long dataLength, unsigned long imageWidth, unsigned long imageHeight, int imageComponents, unsigned long originalWidth, unsigned long originalHeight, unsigned long x, unsigned long y, int level);
    
    virtual void didReceiveImageData(int imageId, unsigned long totalSize, unsigned long receivedSize);
    
    virtual void didFinishDownloadImage(int imageId, unsigned char *data, unsigned long dataLength);
    
    virtual void startTask(Task *task);
    
    virtual void saveImageDataToLocation(unsigned char *data, unsigned long dataLength);
    
private:
    bool decodeJpeg(unsigned char *jpegData, unsigned long dataLength, unsigned char *&outputData, unsigned long &outputDataLength, unsigned long &width, unsigned long &height, int &imageComponents);
    void getRequestBlocks(ScaleInfo *info);
    void calculateImageViewInfo(ScaleInfo *info, int &minX, int &maxX, int &minY, int &maxY, int &level);
    
private:
    int _imageId;
    DMImageInfo _imageInfo;
    int _screenWidth;
    int _screenHeight;
    DMDataManagerCallback *_callback;
    DMImageRequestManager *_requestManager;
    DMBlockDataCache _dataCache;
    
    int _levelCount;
    std::map<unsigned long, int> _requests;
    TaskQueue _calculateQueue;
    TaskQueue _networkQueue;
    TaskQueue _callbackQueue;
    std::mutex _mutex;
};

#endif /* defined(__DataManager__DMImageManager__) */
