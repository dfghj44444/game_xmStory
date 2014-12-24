//
//  TaskQueue.h
//  cqq
//
//  Created by liuchao on 11/26/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#ifndef __cqq__TaskQueue__
#define __cqq__TaskQueue__

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class Task {
public:
    Task();
    virtual ~Task();
    
    int type;
    void *info;
};

class TaskCallback {
public:
    TaskCallback();
    virtual ~TaskCallback() = 0;
    
    virtual void startTask(Task *task) = 0;
};

class TaskQueue {
public:
    TaskQueue();
    ~TaskQueue();
    
    void start();
    
    void addTask(Task *task);
    void addTasks(Task **tasks, unsigned int length);
    void replaceTask(Task *task);
    void replaceTasks(Task **tasks, unsigned int length);
    void removeAllTask();
    
    void setTaskCallback(TaskCallback *callback);
    
    void setMaxTaskCount(unsigned int count);
    void taskFinish();
    
private:
    void threadFunc(std::mutex *mutex, bool *end);
    void clear();
    
private:
    std::vector<Task *> _tasks;
    std::mutex *_mutex;
    std::condition_variable _conditionVar;
    TaskCallback * _callback;
    unsigned int _maxCount;
    unsigned int _currentCount;
    bool *_end;
};

#endif /* defined(__cqq__TaskQueue__) */
