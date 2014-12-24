//
//  HRHTTPRequestIos.cpp
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "HRHTTPRequestIos.h"
#import "HRHTTPRequestObjc.h"
#include "HRHTTPResponseIos.h"
#include "HRHTTPRequestCallback.h"
#include "HRHTTPRequestManager.h"

HRHTTPRequestIos::HRHTTPRequestIos()
{
    _urlString = "";
    _response = nullptr;
    _finishCallback = nullptr;
    _progressCallback = nullptr;
    _objcTarget = nullptr;
    _didRequest = false;
}

HRHTTPRequestIos::~HRHTTPRequestIos()
{
    _cancelRequest();
    if ( _response ) delete _response;
}

bool HRHTTPRequestIos::startSyncRequest(std::string urlString)
{
    std::shared_ptr<HRHTTPRequestIos> ptr;

    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr && !_didRequest.exchange(true) )
    {
        NSString *string = [NSString stringWithUTF8String:urlString.c_str()];
        HRHTTPRequestObjc *requestObjc = [[HRHTTPRequestObjc alloc] init];
        
        _urlString = urlString;
        [requestObjc startSyncRequestWithURLString:string];
        HRHTTPResponseIos *response = new  HRHTTPResponseIos();
        if ( !requestObjc.errorString )
        {
            ((HRHTTPResponseIos *)response)->_responseDataLength = [requestObjc.responseData length];
            if ( [requestObjc.responseData length] > 0 )
            {
                ((HRHTTPResponseIos *)response)->_responseData = new char[[requestObjc.responseData length]];
                memcpy(((HRHTTPResponseIos *)response)->_responseData, [requestObjc.responseData bytes], [requestObjc.responseData length]);
            }
            ((HRHTTPResponseIos *)response)->_MIMEType = [requestObjc.MIMEType UTF8String];
            ((HRHTTPResponseIos *)response)->_statusCode = (unsigned int)requestObjc.statusCode;
        }
        else
        {
            ((HRHTTPResponseIos *)response)->_errorString = [requestObjc.errorString UTF8String];
        }
        _response = response;
        
        return true;
    }
    return false;
}

bool HRHTTPRequestIos::startAsyncRequest(std::string urlString)
{
    std::shared_ptr<HRHTTPRequestIos> ptr;
    
    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr && !_didRequest.exchange(true) )
    {
        bool res;
        NSString *string = [NSString stringWithUTF8String:urlString.c_str()];
        HRHTTPRequestObjc *requestObjc = [[HRHTTPRequestObjc alloc] init];
        std::lock_guard<std::mutex> lock(_mutex);
        
        _urlString = urlString;
        _objcTarget = (void *)CFBridgingRetain(requestObjc);
        [requestObjc setRequest:ptr];
        res = [requestObjc startAsyncRequestWithURLString:string];
        
        return res;
    }
    return false;
}

void HRHTTPRequestIos::cancelRequest()
{
    std::shared_ptr<HRHTTPRequestIos> ptr;

    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr )
        _cancelRequest();
}

void HRHTTPRequestIos::_cancelRequest()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if ( _objcTarget )
    {
        HRHTTPRequestObjc *requestObjc = (HRHTTPRequestObjc *)CFBridgingRelease(_objcTarget);
        [requestObjc cancelRequest];
        requestObjc = nil;
        _objcTarget = nullptr;
    }
}

void HRHTTPRequestIos::setRequestFinishCallback(HRHTTPRequestCallback *callback)
{
    std::shared_ptr<HRHTTPRequestIos> ptr;

    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr )
        _finishCallback = callback;
}

void HRHTTPRequestIos::setRequestProgressCallback(HRHTTPRequestCallback *callback)
{
    std::shared_ptr<HRHTTPRequestIos> ptr;

    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr )
        _progressCallback = callback;
}

std::string HRHTTPRequestIos::urlString()
{
    std::shared_ptr<HRHTTPRequestIos> ptr;

    if ( _hr_requestManager.sharedPtr(this, ptr) && ptr )
        return _urlString;
    return "";
}

HRHTTPResponse *HRHTTPRequestIos::response()
{
    std::shared_ptr<HRHTTPRequestIos> ptr;
    
    if ( _hr_requestManager.sharedPtr(this, ptr) &&  ptr )
        return _response;
    return nullptr;
}

void HRHTTPRequestIos::requestDidFinishLoading(std::string urlString, const void *data, unsigned long dataLength, const char *MIMEType, unsigned int statusCode, const char *errorStr)
{
    HRHTTPRequestCallback *callback = _finishCallback;
    
    _response = new HRHTTPResponseIos();
    if ( errorStr )
    {
        ((HRHTTPResponseIos *)_response)->_errorString = errorStr;
    }
    else
    {
        ((HRHTTPResponseIos *)_response)->_responseDataLength = dataLength;
        if ( dataLength > 0 )
        {
            ((HRHTTPResponseIos *)_response)->_responseData = new char[dataLength];
            memcpy(((HRHTTPResponseIos *)_response)->_responseData, data, dataLength);
        }
        ((HRHTTPResponseIos *)_response)->_MIMEType = MIMEType;
        ((HRHTTPResponseIos *)_response)->_statusCode = statusCode;
    }
    
    if ( callback )
        callback->requestDidFinishLoading(this, _response);
}

void HRHTTPRequestIos::requestDidReceiveData(std::string urlString, unsigned long totalLength, unsigned long receivedLength)
{
    HRHTTPRequestCallback *callback = _progressCallback;
    
    if ( callback )
        callback->requestDidReceiveData(this, totalLength, receivedLength);
}

