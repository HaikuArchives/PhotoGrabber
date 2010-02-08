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
#include "debug.h"
//
FILE *lflevel1;

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
	listofcams.push_back("MSC(Mass Storage) camera");
}

status_t openCamera(void)
{
	// Check Mass Storage Devices
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Start the MSD Interface\n");
		fclose(lflevel1);
	#endif
	msInterface = new MSInterface(msgtarget);
	msInterface->Start();
	return(B_NO_ERROR);
}

status_t closeCamera(void)
{
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Stopping MSD Interface\n");
		fclose(lflevel1);
	#endif
	msInterface->Stop();
	//delete(msInterface);
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - MSD Interface stopped\n");
		fclose(lflevel1);
	#endif
	return(B_NO_ERROR);
}

status_t getNumberofPics(int &number)
{
	int i=0;
	number = 0;
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Get number of pictures\n");
		fclose(lflevel1);
	#endif
	number = msInterface->getNumberOfItems();	
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum)
{
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Set current picture\n");
		fclose(lflevel1);
	#endif
	msInterface->setCurrentItem(picturenum);
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Current picnumber is: %d\n",picturenum);
		fclose(lflevel1);
	#endif
	return(B_NO_ERROR);
}

status_t downloadPicture(BPath savedir, const char *name)
{
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Download picture with name %s\n",name);
		fclose(lflevel1);
	#endif
	msInterface->downloadItem(savedir, name);
	return(B_NO_ERROR);
}

