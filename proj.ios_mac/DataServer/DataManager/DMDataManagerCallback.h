//
//  DMDataManagerCallback.h
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __DataManager__DMDataManagerCallback__
#define __DataManager__DMDataManagerCallback__

#include <string>
#include "DMImageInfo.h"

class DMDataManagerCallback {
public:
    DMDataManagerCallback();
    virtual ~DMDataManagerCallback() = 0;
    
    virtual void didFinishGetImageInfo(DMImageInfo *imageInfo) = 0;
    virtual void didFinishGetImageTileData(int imageId, unsigned char *data, unsigned long dataLength, unsigned long imageWidth, unsigned long imageHeight, int imageComponents, unsigned long originalWidth, unsigned long originalHeight, unsigned long x, unsigned long y, int level) = 0;
    virtual void didReceiveImageData(int imageId, unsigned long totalSize, unsigned long receivedSize) = 0;
    virtual void didFinishDownloadImage(int imageId, unsigned char *data, unsigned long dataLength) = 0;
};

#endif /* defined(__DataManager__DMDataManagerCallback__) */
