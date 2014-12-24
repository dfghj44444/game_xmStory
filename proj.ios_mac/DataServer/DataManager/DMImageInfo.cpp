//
//  DMImageInfo.cpp
//  helloGame
//
//  Created by liuchao on 12/3/14.
//
//

#include "DMImageInfo.h"
#include "json/json.h"
#include <stdlib.h>

static std::string DMItoa(int i)
{
    char tmp[30];
    
    memset(tmp, '\0', 30);
    sprintf(tmp, "%d", i);
    return tmp;
}

DMCreator::DMCreator()
{
    _username = "";
    _userId = -1;
}

DMCreator::~DMCreator()
{
}


DMImageStyle::DMImageStyle()
{
    _name = "";
    _id = -1;
}

DMImageStyle::~DMImageStyle()
{
}


DMImageInfo::DMImageInfo()
{
    _isVaild = false;
}

DMImageInfo::DMImageInfo(std::string jsonString)
{
    Json::Reader reader;
    Json::Value root;
    if ( reader.parse(jsonString, root, false) )
    {
        Json::Value style = root["style"];
        Json::Value creator = root["creator"];
        
        _style._name = style["name"].asString();
        _style._id = style["id"].asInt();
        _addedTime = root["added"].asString();
        _description = root["description"].asString();
        _storeName = root["storeName"].asString();
        _creator._username = creator["username"].asString();
        _creator._userId = creator["id"].asInt();
        _serverIp = root["serverIp"].asString();
        _id = root["id"].asInt();
        _imageName = root["imageName"].asString();
        if ( root["imageHeight"].isString() )
            _imageHeight = atol(root["imageHeight"].asString().c_str());
        else
            _imageHeight = root["imageHeight"].asUInt();
        if ( root["serverPort"].isString() )
            _serverPort = atoi(root["serverPort"].asString().c_str());
        else
            _serverPort = root["serverPort"].asUInt();
        if ( root["imageWidth"].isString() )
            _imageWidth = atol(root["imageWidth"].asString().c_str());
        else
            _imageWidth = root["imageWidth"].asUInt();
        _originName = root["originName"].asString();
        _thumbnail = root["thumbnail"].asString();
        _imageSize = root["imageSize"].asString();
        _isVaild = true;
    }
    else
    {
        _isVaild = false;
    }
}

DMImageInfo::~DMImageInfo()
{
    
}
    
std::string DMImageInfo::baseUrlString()
{
    std::string string = "http://" + _serverIp + ":" + DMItoa(_serverPort) + "/img/http?fname=" + _storeName + "&param=";
    return string;
}

std::string DMImageInfo::downloadUrlString()
{
    std::string string = "http://" + _serverIp + ":" + DMItoa(_serverPort) + "/img/download?fname=" + _originName;
    return string;
}

bool DMImageInfo::isVaild()
{
    return _isVaild;
}

