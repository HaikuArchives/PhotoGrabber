/* bdcpPTP.h */

#ifndef BDCPFUNCTIONS_H
#define BDCPFUNCTIONS_H
#include <Errors.h> // For Return Type
#include <Path.h> // avoids confusion with Path in interface kit.
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <Window.h>
#include <USBKit.h>
#include <vector>
#include <string>
#include <AppFileInfo.h>
#include "PTPFunctions.h"

#define TYPE_USB  1
#define CAM_CONNECTED		'CCON'
#define CAM_DISCONNECTED	'DCON'
#define MSG_UPDATEBAR  0x01012001

using namespace std;
/* BDCP Level 1 Functions */
extern "C" int get_BDCP_API_Revision(void);
extern "C" void getPluginVersion(version_info &);
extern "C" void getSupportedCameras(std::vector<std::string> &listofcams);
extern "C" status_t openCamera(void);
extern "C" status_t closeCamera(void);
extern "C" status_t getNumberofPics(int &);
extern "C" status_t setCurrentPicture(int picturenumber);
extern "C" status_t downloadPicture(BPath savedir);
//Level 2 Functions - Optional
extern "C" status_t deletePicture();
extern "C" status_t takePicture();
extern "C" status_t getImageName(char* &name);
extern "C" status_t getImageSize(int &size);
extern "C" status_t getImageDate(char* &date);
extern "C" BWindow* configurePlugin(BPoint middle);
extern "C" status_t getThumbnail(BBitmap* &picture);
extern "C" status_t getLevel3FunctionNames(map<char*, char*> &menunames);
extern "C" status_t setMessageTarget(BLooper* mtarget);
extern "C" status_t getDeviceType(int &deviceType);
extern "C" status_t getImageHeight(int &height);
extern "C" status_t getImageWidth(int &width);

/* Helper functions */
int CameraIdentify(BUSBDevice* dev);
status_t CameraInit(BUSBDevice* dev);
status_t ParseDir(char *buf, uint32 length);
status_t GetDir(char *path);
status_t connectStateChanged(PTPCamera* camera);

#endif
