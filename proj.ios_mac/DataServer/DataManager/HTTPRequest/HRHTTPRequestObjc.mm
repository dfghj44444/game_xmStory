//
//  HRHTTPRequestObjc.m
//  HTTPRequest
//
//  Created by liuchao on 11/17/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#import "HRHTTPRequestObjc.h"
#include "HRHTTPRequestManager.h"

@interface HRHTTPRequestObjc () <NSURLConnectionDelegate>

@property (strong, nonatomic) NSURLConnection *URLConnection;
@property (copy, nonatomic) NSString *URLString;
@property (strong, nonatomic) NSURLRequest *URLRequest;
@property (strong, nonatomic) NSMutableData *receiveData;
@property (assign, nonatomic) NSUInteger responseMaxLength;
@property (weak, nonatomic) NSThread *syncThread;
@property (assign, nonatomic) BOOL shouldKeepRunning;
@property (strong, nonatomic) NSRecursiveLock *lock;

@end

@implementation HRHTTPRequestObjc
{
    std::weak_ptr<HRHTTPRequestIos> _request;
}

+ (void)networkRequestThreadEntryPoint:(id)__unused object {
    @autoreleasepool {
        [[NSThread currentThread] setName:@"HRHTTPRequest"];
        
        NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
        [runLoop addPort:[NSMachPort port] forMode:NSDefaultRunLoopMode];
        [runLoop run];
    }
}

+ (NSThread *)networkRequestThread {
    static NSThread *_networkRequestThread = nil;
    static dispatch_once_t oncePredicate;
    dispatch_once(&oncePredicate, ^{
        _networkRequestThread = [[NSThread alloc] initWithTarget:self selector:@selector(networkRequestThreadEntryPoint:) object:nil];
        [_networkRequestThread start];
    });
    
    return _networkRequestThread;
}

+ (NSOperationQueue *)requestQueue
{
    static NSOperationQueue *_hr_request_Queue = nil;
    static dispatch_once_t oncePredicate;
    dispatch_once(&oncePredicate, ^{
        _hr_request_Queue = [[NSOperationQueue alloc] init];
    });
    return _hr_request_Queue;
}

- (id)init
{
    self = [super init];
    if ( self ) {
        self.lock = [[NSRecursiveLock alloc] init];
        [self reset];
    }
    return self;
}

- (void)setRequest:(std::weak_ptr<HRHTTPRequestIos>)request
{
    _request = request;
}

- (void)reset
{
    _responseData = nil;
    _URLConnection = nil;
    _URLString = nil;
    _errorString = nil;
    _URLRequest = nil;
    _receiveData = nil;
    _MIMEType = nil;
    _statusCode = 0;
    _responseMaxLength = 0;
}

- (NSURL *)URLWithString:(NSString *)string
{
    if ( string.length < 7 ) return nil;
    
    NSString *prefix = [string substringWithRange:NSMakeRange(0, 7)];
    if ( ![[prefix lowercaseString] isEqualToString:@"http://"] )
        string = [NSString stringWithFormat:@"http://%@", string];
    
    NSArray *components = [string componentsSeparatedByString:@"?"];
    if ( [components count] == 0 ) return nil;
    NSString *baseString = [[components objectAtIndex:0] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    NSMutableString *URLString = [NSMutableString stringWithString:baseString];
    
    if ( [components count] > 1 )
    {
        NSString *paramString = [string substringFromIndex:[baseString length] + 1];
        NSArray *paramPairs = [paramString componentsSeparatedByString:@"&"];
        
        [URLString appendString:@"?"];
        for ( NSString *paramPair in paramPairs )
        {
            NSArray *param = [paramPair componentsSeparatedByString:@"="];
            
            if ( [param count] > 0 )
            {
                NSString *fieldString = [param objectAtIndex:0];
                
                fieldString = (NSString *)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault, (__bridge CFStringRef)fieldString, (__bridge CFStringRef)@"[].", (__bridge CFStringRef)@":/?&=;+!@#$()',*", CFStringConvertNSStringEncodingToEncoding(NSUTF8StringEncoding)));
                [URLString appendString:fieldString];
            }
            if ( [param count] > 1 )
            {
                NSString *valueString = [param objectAtIndex:1];
                
                [URLString appendString:@"="];
                valueString = (NSString *)CFBridgingRelease(CFURLCreateStringByAddingPercentEscapes(kCFAllocatorDefault, (__bridge CFStringRef)valueString, (__bridge CFStringRef)@"|", (__bridge CFStringRef)@":/?&=;+!@#$()',*", CFStringConvertNSStringEncodingToEncoding(NSUTF8StringEncoding)));
                [URLString appendString:valueString];
            }
            [URLString appendString:@"&"];
        }
        [URLString deleteCharactersInRange:NSMakeRange([URLString length] - 1, 1)];
    }
    
    CFURLRef urlRef = CFURLCreateWithBytes(NULL, (const unsigned char *)[URLString UTF8String], [URLString length], CFStringConvertNSStringEncodingToEncoding(NSUTF8StringEncoding), NULL);
    return (NSURL *)CFBridgingRelease(urlRef);
}

- (NSString *)errorStringWithError:(NSError *)error
{
    if ( !error ) return nil;
    return [NSString stringWithFormat:@"Error Domain=%@ Code=%ld %@", error.domain, (long)error.code, error.localizedDescription];
}

- (void)_setShouldKeepRunning:(NSNumber *)shouldKeepRunning
{
    _shouldKeepRunning = [shouldKeepRunning boolValue];
}

- (void)setShouldKeepRunning:(BOOL)shouldKeepRunning
{
    if ( self.syncThread && ![self.syncThread isEqual:[NSThread currentThread]] )
    {
        [self performSelector:@selector(_setShouldKeepRunning:) onThread:self.syncThread withObject:[NSNumber numberWithBool:shouldKeepRunning] waitUntilDone:NO modes:@[NSDefaultRunLoopMode]];
    }
    else
    {
        [self _setShouldKeepRunning:[NSNumber numberWithBool:shouldKeepRunning]];
    }
}

- (BOOL)startSyncRequestWithURLString:(NSString *)string
{
    _shouldKeepRunning = YES;
    self.syncThread = [NSThread currentThread];

    if ( [self startAsyncRequestWithURLString:string] )
    {
        BOOL result = YES;
        
        while (_shouldKeepRunning && result ) {
            result = [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate distantFuture]];
        }
    }
    self.syncThread = nil;
    
    return YES;
}

- (BOOL)startAsyncRequestWithURLString:(NSString *)string
{
    assert( !(self.isExecuting || self.isFinished) );
    
    NSURL *URL = [self URLWithString:string];
    NSURLRequest *request = [NSURLRequest requestWithURL:URL];
    
    [self reset];
    _URLString = string;

    if ( URL && request )
    {
        _URLRequest = request;
        self.runLoopThread = [[self class] networkRequestThread];
        if ( _URLRequest ) [[[self class] requestQueue] addOperation:self];
    }
    else
    {
        _errorString = @"Invalid URL";
        [self didFinshLoading:_errorString];
    }
    
    return YES;
}

- (void)cancelRequest
{
    [self cancel];
}

- (void)didFinshLoading:(NSString *)errorString
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        std::shared_ptr<HRHTTPRequestIos> ptr(_request.lock());
        
        if ( ptr )
        {
            std::string urlString = [_URLString UTF8String];
            const void *data = nullptr;
            unsigned long dataLength = 0;
            const char *errorStr = nullptr;
            const char *MIMEType = nullptr;
            
            if ( _responseData )
            {
                data = [_responseData bytes];
                dataLength = [_responseData length];
            }
            if ( [errorString length] > 0 ) errorStr = [errorString UTF8String];
            if ( [_MIMEType length] > 0 ) MIMEType = [_MIMEType UTF8String];
            ptr->requestDidFinishLoading(urlString, data, dataLength, MIMEType, (unsigned int)_statusCode, errorStr);
        }
    });
    
    self.shouldKeepRunning = NO;
}

- (void)updateReceiveProgress
{
    NSUInteger length = [_receiveData length];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        std::shared_ptr<HRHTTPRequestIos> ptr(_request.lock());
        
        if ( ptr )
        {
            std::string urlString = [_URLString UTF8String];
            ptr->requestDidReceiveData(urlString, _responseMaxLength, length);
        }
    });
}

#pragma mark - Override

- (void)operationDidStart
{
    _URLConnection = [[NSURLConnection alloc] initWithRequest:_URLRequest delegate:self startImmediately:NO];
    
    if ( !_URLConnection )
    {
        NSDictionary *userInfo = [NSDictionary dictionaryWithObject:@"不能建立 NSURLConnection 对象!" forKey:NSLocalizedDescriptionKey];
        NSError *error = [NSError errorWithDomain:@"com.superengine" code:-1000 userInfo:userInfo];
        [self finishWithError:error];
        return;
    }
    
    for (NSString * mode in self.actualRunLoopModes) {
        [_URLConnection scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:mode];
    }
    
    [_URLConnection start];
}

- (void)operationWillFinish
{
    [self didFinshLoading:_errorString];
}

- (void)finishWithError:(NSError *)error
{
    if ( error )
    {
        _errorString = [self errorStringWithError:error];
    }
    
    else
    {
        _errorString = nil;
    }
    [_URLConnection cancel];
    
    [self.lock lock];
    [super finishWithError:error];
    [self.lock unlock];
}

- (void)cancel
{
    [self.lock lock];
    [super cancel];
    [self.lock unlock];
}

#pragma mark - NSURLConnectionDelegate

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    assert(self.isActualRunLoopThread);
    assert(connection == _URLConnection);
#pragma unused(connection)
    assert([response isKindOfClass:[NSHTTPURLResponse class]]);
    
    _MIMEType = [response.MIMEType copy];
    _statusCode = [(NSHTTPURLResponse *)response statusCode];
    NSDictionary *headerFields = [(NSHTTPURLResponse *)response allHeaderFields];
    if ( [headerFields objectForKey:@"Content-Length"] )
        _responseMaxLength = atol([[headerFields objectForKey:@"Content-Length"] UTF8String]);
    else
        _responseMaxLength = 0;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    assert(self.isActualRunLoopThread);
    assert(connection == _URLConnection);
#pragma unused(connection)
    assert(data != nil);
    
    if ( !_receiveData ) _receiveData = [[NSMutableData alloc] initWithCapacity:_responseMaxLength];
    [_receiveData appendData:data];
    [self updateReceiveProgress];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
#pragma unused(connection)
    _responseData = [NSData dataWithData:_receiveData];
    _receiveData = nil;
    [self finishWithError:nil];
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
#pragma unused(connection)
    _receiveData = nil;
    _MIMEType = nil;
    _statusCode = 0;
    [self finishWithError:error];
}

@end
