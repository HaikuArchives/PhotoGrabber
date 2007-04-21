/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
//	Includes
#include <Errors.h> // For Return Type
#include <AppFileInfo.h> // For Version Info
#include <String.h>
#include <cstdio>
#include <Path.h>
#include <string>
#include <vector>
#include <map>
#include <Looper.h>
#ifdef __ZETA__
	#include <usb/USBKit.h>
#else
	#include "USBKit.h"
#endif
//	Local Includes
extern "C" {
#include "ptp.h"
}
#include "PTPInterface.h"

const int32 MSG_UPDATEBAR			= 0x01012001;

BLooper* msgtarget;
class BUSBDevice *appDev;
class Roster	*roster;
int				*handles;
PTPParams		*params;

int currentpicturenumber;
bool haveigotfilenames;
map<int, char*> imagenames;
map<int, int> imagesizes;
map<int, char*> imagedates;
//Level 1 Functions - MUST be implemented fully to be BDCP2 Compliant

extern "C" int get_BDCP_API_Revision(void);
extern "C" void getPluginVersion(version_info &);
extern "C" status_t openCamera(void);
extern "C" status_t closeCamera(void);
extern "C" status_t getNumberofPics(int &);
extern "C" status_t setCurrentPicture(int picturenumber);
extern "C" status_t downloadPicture(BPath savedir);
extern "C" void getSupportedCameras(vector<string> &);
bool saveCamPicture (char *data, long int size, const char filename[255]);

// Messages
#define CAM_CONNECTED				'CCON'
#define CAM_DISCONNECTED			'DCON'

