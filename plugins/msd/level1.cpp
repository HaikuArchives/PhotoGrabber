/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/ 
//		System Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <NodeInfo.h>

//		User Includes
#include "level1.h"
#include "level2.h"
//#include "debug.h"
#include "logger.h"

int get_BDCP_API_Revision(void)
{
	return(2);
}

void getPluginVersion(version_info &ver)
{
	ver.major = 3;
	ver.middle = 0;
	ver.minor = 1;
	ver.variety = 0;
	ver.internal = 0;
	sprintf(ver.short_info,"Jan-Rixt Van Hoye 2010");
	sprintf(ver.long_info,"Mass Storage Cameras Plugin");
}

void getSupportedCameras(std::vector<std::string> & listofcams)
{
	listofcams.push_back("MSC (Mass Storage) camera");
}

status_t openCamera(void)
{
	// Check Mass Storage Devices
	LogDebug("MASS - Start the MSD Interface.");
	msInterface = new MSInterface(msgtarget);
	msInterface->Start();
	return(B_NO_ERROR);
}

status_t closeCamera(void)
{
	LogDebug("MASS - Stopping MSD Interface.");
	msInterface->Stop();
	//delete(msInterface);
	LogDebug("MASS - MSD Interface stopped.");
	return(B_NO_ERROR);
}

status_t getNumberofPics(int &number)
{
	int i=0;
	number = 0;
	LogDebug("MASS - Get number of pictures.");
	number = msInterface->getNumberOfItems();	
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum)
{
	LogDebug("MASS - Set current picture.");
	msInterface->setCurrentItem(picturenum);
	LogDebug("MASS - Current picnumber is: %d.",picturenum);
	return(B_NO_ERROR);
}

status_t downloadPicture(BPath savedir, const char *name)
{
	LogDebug("MASS - Download picture with name %s.",name);
	msInterface->downloadItem(savedir, name);
	return(B_NO_ERROR);
}

