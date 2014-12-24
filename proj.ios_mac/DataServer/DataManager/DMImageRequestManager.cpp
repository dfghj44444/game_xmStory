//
//  DMImageRequestManager.cpp
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "DMImageRequestManager.h"
#include "DMDataManagerCallback.h"
#include <map>
#include <stdlib.h>
#include "json/json.h"
#include "DMImageInfo.h"
#include "HTTPRequest/HRHTTPRequest.h"
#include "HTTPRequest/HRHTTPResponse.h"

using namespace std;

static string BaseURLString = "http://115.238.155.177/img/";

static string DMItoa(int i)
{
    char tmp[30];
    
    memset(tmp, '\0', 30);
    sprintf(tmp, "%d", i);
    return tmp;
}

DMImageRequestManager::DMImageRequestManager(DMDataManagerCallback *requestCallback)
{
    _requestCallback = requestCallback;
}

DMImageRequestManager::~DMImageRequestManager()
{
    _requestCallback = nullptr;
}

void DMImageRequestManager::requestImageInfo(int imageId)
{
    if ( !_requestCallback ) return;
    
    string urlString = "http://182.50.0.212:900/api/view_image/?imageId=" + DMItoa(imageId);
    HRHTTPRequest *request = HRHTTPRequest::CreateHTTPRequest();
    map<string, string> *info = new map<string, string>();
    
    info->insert(map<string, string>::value_type("tag", "1"));
    info->insert(map<string, string>::value_type("imageId", DMItoa(imageId)));
    request->_info = (void *)info;
    request->setRequestFinishCallback(this);
    if ( !request->startAsyncRequest(urlString) )
    {
        _requestCallback->didFinishGetImageInfo(nullptr);
        delete info;
        HRHTTPRequest::ReleaseHTTPRequest(request);
    }
}

void DMImageRequestManager::downloadImage(DMImageInfo *imageInfo)
{
    if ( !_requestCallback || !imageInfo ) return;
    
    HRHTTPRequest *request = HRHTTPRequest::CreateHTTPRequest();
    map<string, string> *info = new map<string, string>();
    string urlString = imageInfo->downloadUrlString();
    
    info->insert(map<string, string>::value_type("tag", "3"));
    info->insert(map<string, string>::value_type("imageId", DMItoa(imageInfo->_id)));
    request->_info = (void *)info;
    request->setRequestFinishCallback(this);
    request->setRequestProgressCallback(this);
    if ( !request->startAsyncRequest(urlString) )
    {
        _requestCallback->didFinishDownloadImage(imageInfo->_id, nullptr, 0);
        delete info;
        HRHTTPRequest::ReleaseHTTPRequest(request);
    }
}

void DMImageRequestManager::requestImageBlock(DMImageInfo *imageInfo, int x, int y, int level, int size)
{
//    http://115.238.155.177/img/http?fname=gsfqs.jpg&param=0|0|0|0
    if ( !_requestCallback || !imageInfo ) return;

    char tmp[100];
    
    memset(tmp, '\0', 100);
    sprintf(tmp, "%d|%d|%d|%d", x, y, level, size);
    
    string urlString = imageInfo->baseUrlString() + tmp;
    HRHTTPRequest *request = HRHTTPRequest::CreateHTTPRequest();
    map<string, string> *info = new map<string, string>();
    
    info->insert(map<string, string>::value_type("tag", "2"));
    info->insert(map<string, string>::value_type("imageId", DMItoa(imageInfo->_id)));
    info->insert(map<string, string>::value_type("x", DMItoa(x)));
    info->insert(map<string, string>::value_type("y", DMItoa(y)));
    info->insert(map<string, string>::value_type("level", DMItoa(level)));
    request->_info = (void *)info;
    request->setRequestFinishCallback(this);
    if ( !request->startAsyncRequest(urlString) )
    {
        _requestCallback->didFinishGetImageTileData(imageInfo->_id, nullptr, 0, 0, 0, 0, 0, 0, x, y, level);
        delete info;
        HRHTTPRequest::ReleaseHTTPRequest(request);
    }
}

void DMImageRequestManager::requestDidFinishLoading(HRHTTPRequest *request, HRHTTPResponse *response)
{
    if ( !_requestCallback )
    {
        HRHTTPRequest::ReleaseHTTPRequest(request);
        return;
    }
    
    map<string, string> *info = (map<string, string> *)request->_info;
    int tag = atoi((*info)["tag"].c_str());
    int imageId = atoi((*info)["imageId"].c_str());
    
    if ( tag == 1 )
    {
        if ( response && !response->errorString() && response->statusCode() == 200 )
        {
            string jsonString = (char *)response->responseData();
            DMImageInfo imageInfo(jsonString);
            if ( _requestCallback )
                _requestCallback->didFinishGetImageInfo(&imageInfo);
        }
        else
        {
            if ( _requestCallback )
                _requestCallback->didFinishGetImageInfo(nullptr);
        }
    }
    else if ( tag == 2 )
    {
        int x = 0, y = 0, level = 0;
        unsigned char *data = nullptr;
        unsigned long length = 0;
        
        x = atoi((*info)["x"].c_str());
        y = atoi((*info)["y"].c_str());
        level = atoi((*info)["level"].c_str());
        if ( response && !response->errorString() && (strcasecmp(response->MIMEType(), "image/jpeg") == 0) && response->statusCode() == 200 )
        {
            data = (unsigned char *)response->responseData();
            length = response->responseDataLength();
        }
        if ( _requestCallback )
            _requestCallback->didFinishGetImageTileData(imageId, data, length, 0, 0, 0, 0, 0, x, y, level);
    }
    else if ( tag == 3 )
    {
        unsigned char *data = nullptr;
        unsigned long length = 0;

        if ( response && !response->errorString() && (strncasecmp(response->MIMEType(), "image/", 6) == 0) && response->statusCode() == 200 )
        {
            data = (unsigned char *)response->responseData();
            length = response->responseDataLength();
        }
        if ( _requestCallback )
            _requestCallback->didFinishDownloadImage(imageId, data, length);
    }
    
    delete info;
    HRHTTPRequest::ReleaseHTTPRequest(request);
}

void DMImageRequestManager::requestDidReceiveData(HRHTTPRequest *request, unsigned long totalLength, unsigned long receivedLength)
{
    map<string, string> *info = (map<string, string> *)request->_info;
    int tag = atoi((*info)["tag"].c_str());
    if ( tag == 3 )
    {
        int imageId = atoi((*info)["imageId"].c_str());
        if ( _requestCallback )
            _requestCallback->didReceiveImageData(imageId, totalLength, receivedLength);
    }
}

