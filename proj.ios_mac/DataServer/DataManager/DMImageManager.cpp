//
//  DMImageManager.cpp
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "DMImageManager.h"
#include "DMImageRequestManager.h"
#include <math.h>
#include <stdlib.h>
#include "jpeg/jpeglib.h"
#include <thread>
#include <assert.h>
#include "SaveImage.h"

#define BlockSize 256

struct BlockInfo {
    int x;
    int y;
    int level;
};

struct ScaleInfo {
    int x;
    int y;
    float scale;
};

struct ImageTileDataInfo {
    int imageId;
    DMBlockData *data;
    unsigned long dataLength;
    unsigned long imageWidth;
    unsigned long imageHeight;
    unsigned long originalWidth;
    unsigned long originalHeight;
    unsigned long x;
    unsigned long y;
    int imageComponents;
    int level;
};


DMImageManager::DMImageManager()
:_dataCache(BlockSize * BlockSize * 3)
{    
    _imageId = -1;
    _screenWidth = 0;
    _screenHeight = 0;
    _callback = 0;
    _levelCount = 0;
    _requestManager = new DMImageRequestManager(this);
    
    _calculateQueue.setMaxTaskCount(1);
    _calculateQueue.setTaskCallback(this);
    _callbackQueue.setMaxTaskCount(1);
    _callbackQueue.setTaskCallback(this);
    _networkQueue.setMaxTaskCount(8);
    _networkQueue.setTaskCallback(this);
    _calculateQueue.start();
    _callbackQueue.start();
    _networkQueue.start();
}

DMImageManager::~DMImageManager()
{
    _calculateQueue.setTaskCallback(nullptr);
    _callbackQueue.setTaskCallback(nullptr);
    _networkQueue.setTaskCallback(nullptr);
    _requestManager->_requestCallback = nullptr;
    delete _requestManager;
    _requestManager = nullptr;
}

void DMImageManager::setImageId(int id)
{
    _imageId = id;
}

void DMImageManager::setScreenSize(int width, int height)
{
    _screenWidth = width;
    _screenHeight = height;
}

void DMImageManager::downloadImage()
{
    if ( _imageInfo.isVaild() && _requestManager )
        _requestManager->downloadImage(&_imageInfo);
}

void DMImageManager::requestImageInfo()
{
    if ( _imageId != -1 )
        _requestManager->requestImageInfo(_imageId);
}

void DMImageManager::passScaleInfo(int x,int y,float scale)
{
    if ( !_imageInfo.isVaild() || _screenWidth * _screenHeight * _imageInfo._imageWidth * _imageInfo._imageHeight == 0 ) return;
    
    ScaleInfo *info = new ScaleInfo;
    Task *task = new Task();
    info->x = x;
    info->y = y;
    info->scale = scale;
    task->type = 1;
    task->info = (void *)info;
    _calculateQueue.replaceTask(task);
}

static bool IsVaildInfo(ScaleInfo *info)
{
    if ( !info || info->scale > 1 || info->scale <= 0 ) return false;
    return true;
}

void DMImageManager::calculateImageViewInfo(ScaleInfo *info, int &blockMinX, int &blockMaxX, int &blockMinY, int &blockMaxY, int &level)
{
    int viewWidht = _imageInfo._imageWidth * info->scale;
    int viewHeight = _imageInfo._imageHeight * info->scale;
    float longerEdge = (viewWidht > viewHeight ? viewWidht : viewHeight) / 256.0;
    level =round( log(longerEdge) / log(2.0f));
    int realX = info->x / info->scale;
    int realY = info->y / info->scale;
    int realWidth = _screenWidth / info->scale;
    int realHeight = _screenHeight / info->scale;
    int minX = realX - realWidth / 2.0;
    int maxX = realX + realWidth / 2.0;
    int minY = realY - realHeight / 2.0;
    int maxY = realY + realHeight / 2.0;
    
    if ( level < 0 ) level = 0;
    if ( level > 8 ) level = 8;
    if ( minX < 0 ) minX = 0;
    if ( maxX >= _imageInfo._imageWidth ) maxX = (int)_imageInfo._imageWidth - 1;
    if ( minY < 0 ) minY = 0;
    if ( maxY >= _imageInfo._imageHeight ) maxY = (int)_imageInfo._imageHeight - 1;
    
    int m = pow(2, _levelCount - level - 1);
    
    blockMinX =  minX / BlockSize / m;
    blockMaxX = maxX / BlockSize / m;
    blockMinY = minY / BlockSize / m;
    blockMaxY = maxY / BlockSize / m;
}

void sortBlocks(std::vector<BlockInfo *> &blocks, int blockMidX, int blockMidY)
{
    if ( blocks.size() == 0 ) return;
    
    std::vector<BlockInfo *> tmp(blocks);
    std::vector<BlockInfo *>::iterator tIt = tmp.begin();
    std::vector<BlockInfo *>::iterator bIt;
    
    blocks.clear();
    for ( ; tIt != tmp.end(); tIt++ )
    {
        BlockInfo *info = *tIt;
        double d = sqrt((info->x - blockMidX) * (info->x - blockMidX) + (info->y - blockMidY) * (info->y - blockMidY));
        
        bIt = blocks.begin();
        for ( ; bIt != blocks.end(); bIt++ )
        {
            BlockInfo *tmpInfo = *bIt;
            double tmpD = sqrt((tmpInfo->x - blockMidX) * (tmpInfo->x - blockMidX) + (tmpInfo->y - blockMidY) * (tmpInfo->y - blockMidY));
            
            if ( d < tmpD )
            {
                blocks.insert(bIt, info);
                break;
            }
        }
        if ( bIt == blocks.end() ) blocks.push_back(info);
    }
}

void DMImageManager::getRequestBlocks(ScaleInfo *info)
{
    if ( !IsVaildInfo(info) ) return;
    
    int blockMinX, blockMaxX, blockMinY, blockMaxY, blockMidX, blockMidY, level;
    std::vector<BlockInfo *> blocks;
    
    calculateImageViewInfo(info, blockMinX, blockMaxX, blockMinY, blockMaxY, level);
    for ( int x = blockMinX; x <= blockMaxX; x++ )
    {
        for ( int y = blockMinY; y <= blockMaxY; y++ )
        {
            unsigned long key = (x << 28) | (y << 14) | level;
            bool didRequest = false;
            
            _mutex.lock();
            didRequest = ( _requests.find(key) != _requests.end() );
            _mutex.unlock();
            if ( !didRequest )
            {
                unsigned long imageWidth, imageHeight;
                int components;
                DMBlockData *data;
                
                if ( _dataCache.cacheData(x, y, level, data, imageWidth, imageHeight, components) && imageWidth * imageHeight * 3 > 0 )
                {
                    unsigned long originalWidth, originalHeight;
                    int blockSize = pow(2.0, _levelCount - level - 1) * BlockSize;
                    Task *task = new Task();
                    ImageTileDataInfo *info = new ImageTileDataInfo();
                    
                    originalWidth = imageWidth * 1.0 / BlockSize * blockSize;
                    originalHeight = imageHeight * 1.0 / BlockSize * blockSize;
                    
                    info->imageId = _imageInfo._id;
                    info->data = data;
                    info->dataLength = imageWidth * imageHeight * 3;
                    info->imageWidth = imageWidth;
                    info->imageHeight = imageHeight;
                    info->imageComponents = components;
                    info->originalWidth = originalWidth;
                    info->originalHeight = originalHeight;
                    info->x = x * blockSize;
                    info->y = y * blockSize;
                    info->level = level;
                    task->type = 3;
                    task->info = (void *)info;
                    _callbackQueue.addTask(task);
                }
                else
                {
                    BlockInfo *info = new BlockInfo();
                    info->x = x;
                    info->y = y;
                    info->level = level;
                    blocks.push_back(info);
                }
            }
        }
    }
    
    _dataCache.setCurrentDisplayInfo(blockMinX - 1, blockMaxX + 1, blockMinY - 1, blockMaxY + 1, level);
    
    blockMidX = (blockMinX + blockMaxX) / 2.0;
    blockMidY = (blockMinY + blockMaxY) / 2.0;
    if ( blocks.size() > 0 )
    {
        sortBlocks(blocks, blockMidX, blockMidY);
        
        std::vector<Task *> tasks;
        std::vector<BlockInfo *>::iterator it = blocks.begin();
        BlockInfo *blockInfo = nullptr;
        
        while (it != blocks.end()) {
            Task *task = new Task();
            
            blockInfo = *it;
            task->type = 2;
            task->info = (void *)blockInfo;
            tasks.push_back(task);
            it++;
        }

        _networkQueue.replaceTasks((Task **)&tasks[0], (unsigned int)tasks.size());
    }
}

void DMImageManager::setDataCallback(DMDataManagerCallback *callback)
{
    _callback = callback;
}

bool DMImageManager::decodeJpeg(unsigned char *jpegData, unsigned long dataLength, unsigned char *&outputData, unsigned long &outputDataLength, unsigned long &width, unsigned long &height, int &imageComponents)
{
    if ( !jpegData || dataLength <= 0 ) return false;
    
    bool bret = false;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpegData, dataLength);
    jpeg_read_header(&cinfo, TRUE);
    cinfo.scale_num = 1;
    cinfo.scale_denom = 1;
    jpeg_start_decompress(&cinfo);
    
    outputDataLength = cinfo.output_width * cinfo.output_height * cinfo.output_components;
    if ( outputDataLength > 0 )
    {
        width = cinfo.output_width;
        height = cinfo.output_height;
        imageComponents = cinfo.output_components;
        outputData = (unsigned char *)malloc(outputDataLength);
        JSAMPROW row_pointer[1];
        while (cinfo.output_scanline < cinfo.output_height)
        {
            row_pointer[0] = (JSAMPROW)outputData + (cinfo.output_scanline)*cinfo.image_width*cinfo.num_components;
            jpeg_read_scanlines(&cinfo,row_pointer , 1);
        }
        bret = true;
    }
    jpeg_destroy_decompress(&cinfo);
    
    return bret;
}

void DMImageManager::didFinishGetImageInfo(DMImageInfo *imageInfo)
{
    if ( imageInfo )
    {
        _imageInfo = *imageInfo;
        float longerEdge = (_imageInfo._imageWidth > _imageInfo._imageHeight ? _imageInfo._imageWidth : _imageInfo._imageHeight) / 256.0;
        _levelCount =round( log(longerEdge) / log(2.0f) ) + 1;
    }
    if ( _callback )
        _callback->didFinishGetImageInfo(imageInfo);
}

void DMImageManager::didFinishGetImageTileData(int imageId, unsigned char *data, unsigned long dataLength, unsigned long imageWidth, unsigned long imageHeight, int imageComponents, unsigned long originalWidth, unsigned long originalHeight, unsigned long x, unsigned long y, int level)
{
    unsigned long key = (x << 28) | (y << 14) | level;
    
    _mutex.lock();
    _requests.erase(key);
    _mutex.unlock();
    
    _networkQueue.taskFinish();
    if ( _callback )
    {
        unsigned char *decodeData = nullptr;
        unsigned long decodeDataLength = 0;
        
        if ( decodeJpeg(data, dataLength, decodeData, decodeDataLength, imageWidth, imageHeight, imageComponents) )
        {
            int blockSize = pow(2.0, _levelCount - level - 1) * BlockSize;
            Task *task = new Task();
            ImageTileDataInfo *info = new ImageTileDataInfo();

            originalWidth = imageWidth * 1.0 / BlockSize * blockSize;
            originalHeight = imageHeight * 1.0 / BlockSize * blockSize;
            DMBlockData *blockData = _dataCache.addCache(decodeData, imageWidth, imageHeight, imageComponents, x, y, level);
            
            free(decodeData);
            if ( !blockData )
            {
                delete task;
                delete info;
                return;
            }
            blockData->retain();
            info->imageId = _imageInfo._id;
            info->data = blockData;
            info->dataLength = imageWidth * imageHeight * 3;
            info->imageWidth = imageWidth;
            info->imageHeight = imageHeight;
            info->imageComponents = imageComponents;
            info->originalWidth = originalWidth;
            info->originalHeight = originalHeight;
            info->x = x * blockSize;
            info->y = y * blockSize;
            info->level = level;
            task->type = 3;
            task->info = (void *)info;
            _callbackQueue.addTask(task);
        }
        else
        {
            _callback->didFinishGetImageTileData(_imageInfo._id, nullptr, 0, 0, 0, 0, 0, 0, x, y, level);
        }
    }
}

void DMImageManager::didReceiveImageData(int imageId, unsigned long totalSize, unsigned long receivedSize)
{
    if ( _callback )
        _callback->didReceiveImageData(imageId, totalSize, receivedSize);
}

void DMImageManager::didFinishDownloadImage(int imageId, unsigned char *data, unsigned long dataLength)
{
    if ( _callback )
        _callback->didFinishDownloadImage(imageId, data, dataLength);
}

void DMImageManager::startTask(Task *task)
{
    if ( !task ) return;
    
    if ( task->type == 1 )
    {
        ScaleInfo *info = (ScaleInfo *)task->info;
        
        if ( info )
        {
            getRequestBlocks(info);
            _calculateQueue.taskFinish();
            delete info;
        }
    }
    else if ( task->type == 2 )
    {
        BlockInfo *info = (BlockInfo *)task->info;
        
        if ( info )
        {
            if ( _requestManager )
            {
                _requestManager->requestImageBlock(&_imageInfo, info->x, info->y, info->level, BlockSize);
                unsigned long key = (info->x << 28) | (info->y << 14) | info->level;
                
                _mutex.lock();
                _requests[key] = 1;
                _mutex.unlock();
            }
            else
            {
                _networkQueue.taskFinish();
            }
            delete info;
        }
    }
    else if ( task->type == 3 )
    {
        ImageTileDataInfo *info = (ImageTileDataInfo *)task->info;
        
        if ( info )
        {
//            dispatch_async(dispatch_get_main_queue(), ^{
                if ( _callback )
                    _callback->didFinishGetImageTileData(_imageInfo._id, info->data->data(), info->dataLength, info->imageWidth, info->imageHeight, info->imageComponents, info->originalWidth, info->originalHeight, info->x, info->y, info->level);
                if ( info->data ) info->data->release();
                delete info;
//            });
            _callbackQueue.taskFinish();
        }
    }
    delete task;
}

void DMImageManager::saveImageDataToLocation(unsigned char *data, unsigned long dataLength)
{
    _saveImageDataToLocation(data, dataLength);
}
