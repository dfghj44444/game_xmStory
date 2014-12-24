//
//  HRHTTPRequestObjc.h
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#import <Foundation/Foundation.h>
#include "HRHTTPRequestIos.h"
#import "QRunLoopOperation.h"

@interface HRHTTPRequestObjc : QRunLoopOperation

- (BOOL)startSyncRequestWithURLString:(NSString *)string;
- (BOOL)startAsyncRequestWithURLString:(NSString *)string;
- (void)cancelRequest;

- (void)setRequest:(std::weak_ptr<HRHTTPRequestIos>)request;

@property (strong, readonly, nonatomic) NSData *responseData;
@property (copy, readonly, nonatomic) NSString *MIMEType;
@property (assign, readonly, nonatomic) NSInteger statusCode;
@property (copy, readonly, nonatomic) NSString *errorString;

@end
