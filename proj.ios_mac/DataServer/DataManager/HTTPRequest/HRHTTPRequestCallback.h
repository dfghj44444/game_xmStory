//
//  HRHTTPRequestCallback.h
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __HTTPRequest__HRHTTPRequestCallback__
#define __HTTPRequest__HRHTTPRequestCallback__

#include <stdio.h>

class HRHTTPRequest;
class HRHTTPResponse;

class HRHTTPRequestCallback {
public:
    virtual ~HRHTTPRequestCallback();
    virtual void requestDidFinishLoading(HRHTTPRequest *request, HRHTTPResponse *response) = 0;
    virtual void requestDidReceiveData(HRHTTPRequest *request, unsigned long totalLength, unsigned long receivedLength) = 0;
};

#endif /* defined(__HTTPRequest__HRHTTPRequestCallback__) */
