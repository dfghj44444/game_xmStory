//
//  DMBlockDataCache.h
//  helloGame
//
//  Created by liuchao on 11/24/14.
//
//

#ifndef __helloGame__DMBlockDataCache__
#define __helloGame__DMBlockDataCache__

#include <stdio.h>
#include <map>
#include <vector>
#include <mutex>

class DMRef {
public:
    DMRef();
    virtual ~DMRef();
    
    void retain();
    void release();
    
private:
    std::atomic<unsigned int> _referenceCount;
};

class DMBlockKey {
public:
    DMBlockKey();
    DMBlockKey(const DMBlockKey &key);
    ~DMBlockKey();
    
    DMBlockKey &operator= (const DMBlockKey &key);
    bool operator < (const DMBlockKey &key) const;
    
public:
    unsigned long _x;
    unsigned long _y;
    int _level;
};

class DMBlockData : public DMRef {
public:
    DMBlockData(unsigned long length);
    virtual ~DMBlockData();
    
    unsigned char *data() { return _data; };
    unsigned long dataLength() { return _dataLength; };
    
private:
    unsigned char *_data;
    unsigned long _dataLength;
};

class DMBlockDataCache {
public:
    DMBlockDataCache(unsigned long blockDataLength);
    virtual ~DMBlockDataCache();
        
    DMBlockData *addCache(unsigned char *data, unsigned long width, unsigned long height, int components, unsigned long x, unsigned long y, int level);
    bool cacheData(unsigned long x, unsigned long y, int level, DMBlockData *&data, unsigned long &width, unsigned long &height, int &components);
    
    void setCurrentDisplayInfo(int minX, int maxX, int minY, int maxY, int level);
    
private:
    std::map<DMBlockKey, DMBlockData *> _cache;
    std::vector<DMBlockData *> _reuseMem;
    unsigned long _blockDataLength;
    std::mutex _cacheMutex;
};

#endif /* defined(__helloGame__DMBlockDataCache__) */
