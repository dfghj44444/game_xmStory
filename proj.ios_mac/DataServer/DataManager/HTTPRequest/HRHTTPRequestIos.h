//
//  HRHTTPRequestIos.h
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __HTTPRequest__HRHTTPRequestIos__
#define __HTTPRequest__HRHTTPRequestIos__

#include <stdio.h>
#include <mutex>
#include <thread>
#include "HRHTTPRequest.h"

class HRHTTPRequestCallback;

class HRHTTPRequestIos : public HRHTTPRequest {
public:
    HRHTTPRequestIos();
    virtual ~HRHTTPRequestIos();
    
    virtual bool startSyncRequest(std::string urlString);
    virtual bool startAsyncRequest(std::string urlString);
    virtual void cancelRequest();
    
    virtual void setRequestFinishCallback(HRHTTPRequestCallback *callback);
    virtual void setRequestProgressCallback(HRHTTPRequestCallback *callback);
    
    virtual std::string urlString();
    
    virtual HRHTTPResponse *response();
    
    void requestDidFinishLoading(std::string urlString, const void *data, unsigned long dataLength, const char *MIMEType, unsigned int statusCode, const char *errorStr);
    void requestDidReceiveData(std::string urlString, unsigned long totalLength, unsigned long receivedLength);
    
private:
    void _cancelRequest();
    
private:
    std::string _urlString;
    HRHTTPResponse *_response;
    HRHTTPRequestCallback *_finishCallback;
    HRHTTPRequestCallback *_progressCallback;
    void *_objcTarget;
    std::atomic<bool> _didRequest;
    std::mutex _mutex;
};

#endif /* defined(__HTTPRequest__HRHTTPRequestIos__) */
