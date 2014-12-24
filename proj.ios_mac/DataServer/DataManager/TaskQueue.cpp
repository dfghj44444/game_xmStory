//
//  TaskQueue.cpp
//  cqq
//
//  Created by liuchao on 11/26/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "TaskQueue.h"

///////////////////////////////// Task ////////////////////////////////////////

Task::Task()
:type(0), info(nullptr)
{
}

Task::~Task()
{
}

///////////////////////////////// TaskCallback ////////////////////////////////////////

TaskCallback::TaskCallback()
{
}

TaskCallback::~TaskCallback()
{
}

///////////////////////////////// TaskQueue ////////////////////////////////////////

TaskQueue::TaskQueue()
:_callback(nullptr), _maxCount(4), _currentCount(0)
{
    _end = new bool;
    *_end = false;
    _mutex = new std::mutex();
}

TaskQueue::~TaskQueue()
{
    fprintf(stdout, "dealloc %p\n", this);
    _mutex->lock();
    clear();
    _mutex->unlock();
    *_end = true;
    _conditionVar.notify_one();
}

void TaskQueue::start()
{
    std::thread thread(std::bind(&TaskQueue::threadFunc, this, _mutex, _end));
    thread.detach();
}

void TaskQueue::addTask(Task *task)
{
    if ( !task ) return;
    
    _mutex->lock();
    _tasks.push_back(task);
    _mutex->unlock();
    _conditionVar.notify_one();
}

void TaskQueue::addTasks(Task **tasks, unsigned int length)
{
    if ( !tasks || length == 0 ) return;
    
    _mutex->lock();
    for ( unsigned int idx = 0; idx < length; idx++ )
    {
        Task *task = *(tasks + idx);
        if ( task ) _tasks.push_back(task);
    }
    _mutex->unlock();
    _conditionVar.notify_one();
}

void TaskQueue::replaceTask(Task *task)
{
    _mutex->lock();
    clear();
    if ( task )
        _tasks.push_back(task);
    _mutex->unlock();
    if ( task )
        _conditionVar.notify_one();
}

void TaskQueue::replaceTasks(Task **tasks, unsigned int length)
{
    _mutex->lock();
    clear();
    if ( tasks )
    {
        for ( unsigned int idx = 0; idx < length; idx++ )
        {
            Task *task = *(tasks + idx);
            if ( task ) _tasks.push_back(task);
        }
    }
    _mutex->unlock();
    if ( tasks && length > 0 )
        _conditionVar.notify_one();
}

void TaskQueue::removeAllTask()
{
    _mutex->lock();
    clear();
    _mutex->unlock();
}

void TaskQueue::setTaskCallback(TaskCallback *callback)
{
    _callback = callback;
}

void TaskQueue::setMaxTaskCount(unsigned int count)
{
    _maxCount = count;
}

void TaskQueue::taskFinish()
{
    _mutex->lock();
    _currentCount--;
    _mutex->unlock();
    _conditionVar.notify_one();
}

void TaskQueue::threadFunc(std::mutex *mutex, bool *end)
{
    while (!(*end)) {
        Task *task = nullptr;
        
        std::unique_lock<std::mutex> uniqueLock(*mutex, std::defer_lock);
        
        uniqueLock.lock();
        if ( _currentCount < _maxCount && _tasks.size() > 0 )
        {
            std::vector<Task *>::iterator it = _tasks.begin();
            
            task = *it;
            _tasks.erase(it);
            _currentCount++;
        }
        else
        {
            _conditionVar.wait(uniqueLock);
        }
        uniqueLock.unlock();

        if ( task )
        {
            TaskCallback *callback = _callback;
            
            if ( callback )
                callback->startTask(task);
        }
    }
    delete end;
    delete mutex;
}

void TaskQueue::clear()
{
    std::vector<Task *>::iterator it = _tasks.begin();
    for ( ; it != _tasks.end(); it++ )
    {
        Task *task = *it;
        if ( task ) delete task;
    }
    _tasks.clear();
}

