//
//  DMImageRequestManager.h
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __DataManager__DMImageRequestManager__
#define __DataManager__DMImageRequestManager__

#include <stdio.h>
#include <string>
#include "HTTPRequest/HRHTTPRequestCallback.h"

#define MAX_REQUEST_COUNT 10

class DMImageInfo;
class HRHTTPRequest;
class DMDataManagerCallback;

class DMImageRequestManager : public HRHTTPRequestCallback {
public:
    DMImageRequestManager(DMDataManagerCallback *requestCallback);
    ~DMImageRequestManager();
    
    void requestImageInfo(int imageId);
    void downloadImage(DMImageInfo *imageInfo);
    void requestImageBlock(DMImageInfo *imageInfo, int x, int y, int level, int size);
    
    virtual void requestDidFinishLoading(HRHTTPRequest *request, HRHTTPResponse *response);
    virtual void requestDidReceiveData(HRHTTPRequest *request, unsigned long totalLength, unsigned long receivedLength);
    
public:
    DMDataManagerCallback *_requestCallback;
};

#endif /* defined(__DataManager__DMImageRequestManager__) */
