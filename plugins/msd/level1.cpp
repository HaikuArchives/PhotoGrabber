/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
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
//		USB Roster class
class Roster : public BUSBRoster
{
public:
virtual status_t DeviceAdded(BUSBDevice *dev)
{
	// Initialize the USB device
	int i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	
	for(i=0;i<(int)dev->CountConfigurations();i++)
	{
		conf = dev->ConfigurationAt(i);
		if(conf)
		{
			for(j=0;j<(int)conf->CountInterfaces();j++)
			{
				ifc = conf->InterfaceAt(j);	
				if(ifc)
					if(ifc->Class() == 8)
					{
						#ifdef DEBUG
							lflevel1 = fopen(LOGFILE,"a");
							fprintf(lflevel1,"MS: Device %s found!\n",dev->ProductString());
							fclose(lflevel1);
						#endif
						appDev = dev;
						if(appDev->InitCheck() || appDev->SetConfiguration(appDev->ConfigurationAt(0)))
							logError(MSCAM_DEV_NO_FIND);
						else
						{
							// create the mass storage interface
							mscam = new MSDInterface(appDev);
							#ifdef DEBUG
								lflevel1 = fopen(LOGFILE,"a");
								fprintf(lflevel1,"MS: Mount device %s.\n",dev->ProductString());
								fclose(lflevel1);
							#endif
							mscam->Mount();
							// send a message to the system core
							BMessage *core_msg;
							core_msg = new BMessage(CAM_CONNECTED);
							core_msg->AddString("product",dev->ProductString());
							if(msgtarget != NULL)
							{
								#ifdef DEBUG
									lflevel1 = fopen(LOGFILE,"a");
									fprintf(lflevel1,"MS: Send message to the system core\n");
									fclose(lflevel1);
								#endif
								msgtarget->PostMessage(core_msg);
							}
						}
					}	
			}
		}
	}
	
	return B_OK;
}
virtual void DeviceRemoved(BUSBDevice *dev)
{
	int i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	
	for(i=0;i<(int)dev->CountConfigurations();i++)
	{
		conf = dev->ConfigurationAt(i);
		if(conf)
		{
			for(j=0;j<(int)conf->CountInterfaces();j++)
			{
				ifc = conf->InterfaceAt(j);	
				if(ifc)
					if(ifc->Class() == 8)
					{
						appDev = NULL;
						// send a message to the system core
						BMessage *core_msg;
						core_msg = new BMessage(CAM_DISCONNECTED);
						core_msg->AddString("product",dev->ProductString());
						if(msgtarget != NULL)
						{
							#ifdef DEBUG
								lflevel1 = fopen(LOGFILE,"a");
								fprintf(lflevel1,"MS: Send message to the system core\n");
								fclose(lflevel1);
							#endif
							msgtarget->PostMessage(core_msg);
						}
					}
			}
		}
	}
}
};

int get_BDCP_API_Revision(void)
{
	return(2);
}

void getPluginVersion(version_info &ver)
{
	ver.major = 2;
	ver.middle = 0;
	ver.minor = 0;
	ver.variety = 0;
	ver.internal = 0;
	sprintf(ver.short_info,"Jan-Rixt Van Hoye 2008");
	sprintf(ver.long_info,"BDCP Mass Storage Cameras Plugin");
}

void getSupportedCameras(std::vector<std::string> & listofcams)
{
	listofcams.push_back("MSC(Mass Storage) camera");
}

status_t openCamera(void)
{
	// Check USB Devices
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Start the roster\n");
		fclose(lflevel1);
	#endif
	roster = new Roster;
	roster->Start();
	return(B_NO_ERROR);
}

status_t closeCamera(void)
{
	// Close the camera
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Close MSD plugin\n");
		fclose(lflevel1);
	#endif
	delete(mscam);
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Stopping USB Roster\n");
		fclose(lflevel1);
	#endif
	roster->Stop();
	delete(roster);
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - USB Roster stopped\n");
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
	number = mscam->getNumberOfItems();	
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum)
{
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"MS - Set current picture\n");
		fclose(lflevel1);
	#endif
	mscam->setCurrentItem(picturenum);
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
		fprintf(lflevel1,"MS - Download pictures\n");
		fclose(lflevel1);
	#endif
	mscam->downloadItem(savedir, name);
	return(B_NO_ERROR);
}

