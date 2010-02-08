/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
#include <Errors.h>
#include <map>
#include <Bitmap.h>
#include "MSInterface.h"


//extern int currentitemhandle;
extern int *handles;
extern bool haveigotfilenames;
extern std::map<int, char*> imagenames;
extern std::map<int, int> imagesizes;
extern std::map<int, char*> imagedates;

//Level 2 Functions - Optional
extern "C" status_t deletePicture();
extern "C" status_t takePicture();
extern "C" status_t getImageName(char* &name);
extern "C" status_t getImageSize(int &size);
extern "C" status_t getImageDate(char* &date);
extern "C" status_t getThumbnail(BBitmap* &);
extern "C" status_t getLevel3FunctionNames(std::map<char*, char*> &menunames);
extern "C" status_t setMessageTarget(BLooper* mtarget);
//Functions for BDCP3
extern "C" status_t getDeviceType(int &type);
extern "C" status_t getImageHeight(int &height);
extern "C" status_t getImageWidth(int &width);

