//
//  HRHTTPResponseIos.cpp
//  HTTPRequest
//
//  Created by liuchao on 11/18/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "HRHTTPResponseIos.h"

HRHTTPResponseIos::HRHTTPResponseIos()
{
    _responseData = nullptr;
    _responseDataLength = 0;
    _MIMEType = "";
    _statusCode = 0;
    _errorString = "";
}

HRHTTPResponseIos::~HRHTTPResponseIos()
{
    if ( _responseData ) delete [] (char *)_responseData;
}

void *HRHTTPResponseIos::responseData()
{
    return _responseData;
}

unsigned long HRHTTPResponseIos::responseDataLength()
{
    return _responseDataLength;
}

const char *HRHTTPResponseIos::MIMEType()
{
    return _MIMEType.c_str();
}

unsigned int HRHTTPResponseIos::statusCode()
{
    return _statusCode;
}

const char *HRHTTPResponseIos::errorString()
{
    if ( _errorString.length() > 0 )
        return _errorString.c_str();
    else
        return nullptr;
}

