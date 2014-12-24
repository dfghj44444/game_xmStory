//
//  HRHTTPResponseIos.h
//  HTTPRequest
//
//  Created by liuchao on 11/18/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __HTTPRequest__HRHTTPResponseIos__
#define __HTTPRequest__HRHTTPResponseIos__

#include "HRHTTPResponse.h"
#include "HRHTTPRequestIos.h"
#include <string>

class HRHTTPResponseIos : public HRHTTPResponse {
    friend HRHTTPRequestIos;
public:
    HRHTTPResponseIos();
    virtual ~HRHTTPResponseIos();
    
    virtual void *responseData();
    virtual unsigned long responseDataLength();
    virtual const char *MIMEType();
    virtual unsigned int statusCode();
    
    virtual const char *errorString();
    
public:
    void *_responseData;
    unsigned long _responseDataLength;
    std::string _MIMEType;
    unsigned int _statusCode;
    std::string _errorString;
};

#endif /* defined(__HTTPRequest__HRHTTPResponseIos__) */
