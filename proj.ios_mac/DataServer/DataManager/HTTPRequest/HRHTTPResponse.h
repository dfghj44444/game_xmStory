//
//  HRHTTPResponse.h
//  HTTPRequest
//
//  Created by liuchao on 11/18/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __HTTPRequest__HRHTTPResponse__
#define __HTTPRequest__HRHTTPResponse__

#include <stdio.h>

class HRHTTPResponse {
public:
    virtual ~HRHTTPResponse() = 0;
    
    virtual void *responseData() = 0;
    virtual unsigned long responseDataLength() = 0;
    virtual const char *MIMEType() = 0;
    virtual unsigned int statusCode() = 0;
    
    virtual const char *errorString() = 0;
};

#endif /* defined(__HTTPRequest__HRHTTPResponse__) */
