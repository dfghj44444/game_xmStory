//
//  HRHTTPRequest.m
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "HRHTTPRequest.h"
#include "HRHTTPRequestIos.h"
#include <map>
#include "HRHTTPRequestManager.h"

HRHTTPRequest::~HRHTTPRequest()
{
    _info = nullptr;
}

HRHTTPRequest *HRHTTPRequest::CreateHTTPRequest()
{
    HRHTTPRequestIos *request = new HRHTTPRequestIos();
    _hr_requestManager.addRequest(request);
    return request;
}

void HRHTTPRequest::ReleaseHTTPRequest(HRHTTPRequest *request)
{
    _hr_requestManager.removeRequest((HRHTTPRequestIos *)request);
}


