//
//  DMBlockDataCache.cpp
//  helloGame
//
//  Created by liuchao on 11/24/14.
//
//

#include "DMBlockDataCache.h"
#include <assert.h>

DMRef::DMRef()
:_referenceCount(1)
{
}

DMRef::~DMRef()
{
}

void DMRef::retain()
{
    unsigned int referenceCount = _referenceCount++;
    assert(referenceCount > 0);
}

void DMRef::release()
{
    unsigned int referenceCount = --_referenceCount;
    if ( referenceCount == 0 )
        delete this;
}

/////////////////////////////////////////////////////////////////

DMBlockKey::DMBlockKey()
{
    _x = 0;
    _y = 0;
    _level = 0;
}

DMBlockKey::DMBlockKey(const DMBlockKey &key)
{
    _x = key._x;
    _y = key._y;
    _level = key._level;
}

DMBlockKey::~DMBlockKey()
{
}

DMBlockKey &DMBlockKey::operator=(const DMBlockKey &key)
{
    _x = key._x;
    _y = key._y;
    _level = key._level;
    return *this;
}

bool DMBlockKey::operator<(const DMBlockKey &key) const
{
    if ( _level < key._level )
        return true;
    if ( _level > key._level )
        return false;
    if ( _x < key._x )
        return true;
    if ( _x > key._x )
        return false;
    if ( _y < key._y )
        return true;
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////

DMBlockData::DMBlockData(unsigned long length)
{
    _dataLength = length;
    if ( _dataLength > 0 )
        _data = new unsigned char[_dataLength];
    else
        _data = nullptr;
}

DMBlockData::~DMBlockData()
{
    if ( _data )
        delete [] _data;
    _data = nullptr;
    _dataLength = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////

DMBlockDataCache::DMBlockDataCache(unsigned long length)
{
    _blockDataLength = length;
}

DMBlockDataCache::~DMBlockDataCache()
{
    std::vector<DMBlockData *>::iterator vIt = _reuseMem.begin();
    
    _cacheMutex.lock();
    for ( ; vIt != _reuseMem.end(); vIt++ )
    {
        DMBlockData *data = *vIt;
        if ( data ) data->release();
    }
    _reuseMem.clear();
    
    std::map<DMBlockKey, DMBlockData *>::iterator mIt = _cache.begin();
    
    for ( ; mIt != _cache.end(); mIt++ )
    {
        DMBlockData *data = mIt->second;
        if ( data ) data->release();
    }
    _cache.clear();
    _cacheMutex.unlock();
}

void DMBlockDataCache::setCurrentDisplayInfo(int minX, int maxX, int minY, int maxY, int level)
{
    std::map<DMBlockKey, DMBlockData *>::iterator it = _cache.begin();

    _cacheMutex.lock();
    for ( ; it != _cache.end(); )
    {
        DMBlockKey key = it->first;
        
        if ( key._level != level
            || (key._x < minX)
            || key._x > maxX
            || key._y < minY
            || key._y > maxY
            )
        {
            std::map<DMBlockKey, DMBlockData *>::iterator removeIt = it;
            DMBlockData *buffer = removeIt->second;
            
            it++;
            _cache.erase(removeIt);
            if ( buffer ) _reuseMem.push_back(buffer);
        }
        else
        {
            it++;
        }
    }
    _cacheMutex.unlock();
}

DMBlockData *DMBlockDataCache::addCache(unsigned char *data, unsigned long width, unsigned long height, int components, unsigned long x, unsigned long y, int level)
{
    unsigned long dataLength = width * height * components;
    if ( !data || dataLength <= 0 || dataLength > _blockDataLength ) return nullptr;
    
    DMBlockData *buffer = nullptr;
    DMBlockKey key;

    _cacheMutex.lock();
    std::vector<DMBlockData *>::iterator it = _reuseMem.begin();
    if ( it != _reuseMem.end() )
    {
        buffer = *it;
        _reuseMem.erase(it);
    }
    _cacheMutex.unlock();
    
    if ( buffer == nullptr )
        buffer = new DMBlockData(_blockDataLength + sizeof(unsigned long) * 2 + sizeof(int));
    if ( buffer->data() && buffer->dataLength() >= _blockDataLength + sizeof(unsigned long) * 2 + sizeof(int))
    {
        memcpy(buffer->data(), data, dataLength);
        memcpy(buffer->data() + _blockDataLength, (const void *)&width, sizeof(unsigned long));
        memcpy(buffer->data() + _blockDataLength + sizeof(unsigned long), (const void *)&height, sizeof(unsigned long));
        memcpy(buffer->data() + _blockDataLength + sizeof(unsigned long) * 2, (const void *)&components, sizeof(int));
    }
    else
    {
        assert(0);
    }
    key._x = x;
    key._y = y;
    key._level = level;
    
    _cacheMutex.lock();
    _cache.insert(std::map<DMBlockKey, DMBlockData *>::value_type(key, buffer));
    _cacheMutex.unlock();
    
    return buffer;
}

bool DMBlockDataCache::cacheData(unsigned long x, unsigned long y, int level, DMBlockData *&data, unsigned long &width, unsigned long &height, int &components)
{
    DMBlockKey key;
    std::map<DMBlockKey, DMBlockData *>::iterator it;
    
    data = nullptr;
    key._x = x;
    key._y = y;
    key._level = level;
    
    _cacheMutex.lock();
    it = _cache.find(key);
    if ( it != _cache.end() )
    {
        data = it->second;
        if ( data ) data->retain();
    }
    _cacheMutex.unlock();
    
    if ( data )
    {
        unsigned long w, h;
        int c;
        memcpy((void *)&w, (const void *)(data->data() + _blockDataLength), sizeof(unsigned long));
        memcpy((void *)&h, (const void *)(data->data() + _blockDataLength + sizeof(unsigned long)), sizeof(unsigned long));
        memcpy((void *)&c, (const void *)(data->data() + _blockDataLength + sizeof(unsigned long) * 2), sizeof(int));
        width =w;
        height = h;
        components = c;
        return true;
    }
    return false;
}

