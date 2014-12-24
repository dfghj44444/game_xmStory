//
//  DMImageInfo.h
//  helloGame
//
//  Created by liuchao on 12/3/14.
//
//

#ifndef __helloGame__DMImageInfo__
#define __helloGame__DMImageInfo__

#include <string>

class DMCreator {
public:
    DMCreator();
    virtual ~DMCreator();
    
    std::string _username;
    int _userId;
};

class DMImageStyle {
public:
    DMImageStyle();
    virtual ~DMImageStyle();
    
    std::string _name;
    int _id;
};

class DMImageInfo {
public:
    DMImageInfo();
    DMImageInfo(std::string jsonString);
    virtual ~DMImageInfo();
    
    std::string baseUrlString();
    std::string downloadUrlString();
    
    bool isVaild();
    
    DMImageStyle _style;
    std::string _addedTime;
    std::string _description;
    std::string _storeName;
    DMCreator _creator;
    std::string _serverIp;
    unsigned int _serverPort;
    int _id;
    std::string _imageName;
    unsigned long _imageWidth;
    unsigned long _imageHeight;
    std::string _originName;
    std::string _thumbnail;
    std::string _imageSize;
    bool _isVaild;
};

#endif /* defined(__helloGame__DMImageInfo__) */
