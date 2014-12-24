//
//  HRHTTPRequest.h
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __HR_HTTP_REQUEST_H__
#define __HR_HTTP_REQUEST_H__

#include <string>

class HRHTTPResponse;
class HRHTTPRequestCallback;

class HRHTTPRequest {
public:
    virtual ~HRHTTPRequest();
    
    virtual bool startSyncRequest(std::string urlString) = 0;
    virtual bool startAsyncRequest(std::string urlString) = 0;
    virtual void cancelRequest() = 0;
    
    virtual void setRequestFinishCallback(HRHTTPRequestCallback *callback) = 0;
    virtual void setRequestProgressCallback(HRHTTPRequestCallback *callback) = 0;
    
    virtual std::string urlString() = 0;
    
    virtual HRHTTPResponse *response() = 0;
    
    static HRHTTPRequest *CreateHTTPRequest();
    static void ReleaseHTTPRequest(HRHTTPRequest *request);
    
public:
    void *_info;
};

#endif
