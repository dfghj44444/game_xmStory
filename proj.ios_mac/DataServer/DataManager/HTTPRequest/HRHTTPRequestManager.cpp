//
//  HRHTTPRequestManager.cpp
//  helloGame
//
//  Created by liuchao on 12/5/14.
//
//

#include "HRHTTPRequestManager.h"
#include "HRHTTPRequestIos.h"

HRHTTPRequestManager _hr_requestManager;

HRHTTPRequestManager::HRHTTPRequestManager()
{
}

HRHTTPRequestManager::~HRHTTPRequestManager()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _requests.clear();
}

void HRHTTPRequestManager::addRequest(HRHTTPRequestIos *request)
{
    if ( request )
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _requests.insert(std::map<HRHTTPRequestIos *, std::shared_ptr<HRHTTPRequestIos>>::value_type(request, std::shared_ptr<HRHTTPRequestIos>(request)));
    }
}

void HRHTTPRequestManager::removeRequest(HRHTTPRequestIos *request)
{
    if ( request )
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if ( _requests.find(request) != _requests.end() )
            _requests.erase(request);
    }
}

bool HRHTTPRequestManager::sharedPtr(HRHTTPRequestIos *request, std::shared_ptr<HRHTTPRequestIos> &output)
{
    if ( request )
    {
        std::lock_guard<std::mutex> lock(_mutex);
        std::map<HRHTTPRequestIos *, std::shared_ptr<HRHTTPRequestIos>>::iterator it = _requests.find(request);
        if ( it != _requests.end() )
        {
            output = it->second;
            return true;
        }
    }
    return false;
}

