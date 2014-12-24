//
//  HRHTTPRequestManager.h
//  helloGame
//
//  Created by liuchao on 12/5/14.
//
//

#ifndef __helloGame__HRHTTPRequestManager__
#define __helloGame__HRHTTPRequestManager__

#include <memory>
#include <map>
#include <mutex>

class HRHTTPRequestIos;

class HRHTTPRequestManager {
public:
    HRHTTPRequestManager();
    virtual ~HRHTTPRequestManager();
    
    void addRequest(HRHTTPRequestIos *request);
    void removeRequest(HRHTTPRequestIos *request);
    
    bool sharedPtr(HRHTTPRequestIos *request, std::shared_ptr<HRHTTPRequestIos> &output);
    
private:
    std::mutex _mutex;
    std::map<HRHTTPRequestIos *, std::shared_ptr<HRHTTPRequestIos>> _requests;
};

extern HRHTTPRequestManager _hr_requestManager;


#endif /* defined(__helloGame__HRHTTPRequestManager__) */
