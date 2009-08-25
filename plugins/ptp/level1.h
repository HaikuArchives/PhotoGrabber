/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//	Includes
#include <Errors.h> // For Return Type
#include <AppFileInfo.h> // For Version Info
#include <String.h>
#include <stdio.h>
#include <stdlib.h>
#include <Path.h>
#include <string>
#include <vector>
#include <map>
#include <Looper.h>
#include "PTPInterface.h"
#include <USBKit.h>
//	Local Includes
extern "C" {
#include "libptp/ptp.h"
}


const int32 MSG_UPDATEBAR			= 0x01012001;

BLooper* msgtarget;
//class BUSBDevice *appDev;
USBCameraDevice * cameraDevice;
class Roster	*roster;
int				*handles;
PTPParams		*params;

int currentpicturenumber;
//bool haveigotfilenames;
//std::map<int, char*> imagenames;
//std::map<int, int> imagesizes;
//std::map<int, char*> imagedates;
//Level 1 Functions - MUST be implemented fully to be BDCP2 Compliant

extern "C" int get_BDCP_API_Revision(void);
extern "C" void getPluginVersion(version_info &);
extern "C" status_t openCamera(void);
extern "C" status_t closeCamera(void);
extern "C" status_t getNumberofPics(int &);
extern "C" status_t setCurrentPicture(int picturenumber);
extern "C" status_t downloadPicture(BPath savedir, const char *name);
extern "C" void getSupportedCameras(std::vector<std::string> &);
bool saveCamPicture (unsigned char *data, long int size,uint16_t type, const char *filename);

// Messages
#define CAM_CONNECTED				'CCON'
#define CAM_DISCONNECTED			'DCON'

