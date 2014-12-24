//
//  DMDataManager.cpp
//  DataManager
//
//  Created by liuchao on 11/19/14.
//  Copyright (c) 2014 chaoliu. All rights reserved.
//

#include "DMDataManager.h"
#include "DMImageManager.h"

DMDataManager::DMDataManager()
{
}

DMDataManager::~DMDataManager()
{
}

DMDataManager *DMDataManager::CreateDataManager()
{
    return new DMImageManager();
}

void DMDataManager::ReleaseDataManager(DMDataManager *dataManager)
{
    if ( dataManager )
        delete dataManager;
}
