/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File definition
#ifndef CAMINTF_H
#define CAMINTF_H

using namespace std;
//		Includes
#include <map>
#include <Looper.h>
#include <Entry.h>
#include <Path.h>
#include <image.h>
#include <AppFileInfo.h>
#include <Window.h>
#include <vector>
#include <string>

//		Local Includes
#include "core_global.h"

enum pluginCaps {
	fn_revision,
	fn_supcams,
	fn_pluginVersion,
	fn_openCamera,
	fn_closeCamera,
	fn_numberOfPictures,
	fn_currentPicture,
	fn_downloadPicture,
	fn_deletePicture,
	fn_takePicture,
	fn_imageName,
	fn_imageSize,
	fn_imageDate,
	fn_configurePlugin,
	fn_messageTarget,
	fn_level3Functions,
	fn_thumbnail,
	fn_imageHeight,
	fn_imageWidth,
	fn_deviceType,

	fns_count
};

typedef status_t (*level3Func)();


//		CameraInterface class
class CamInterface {
public:
                                    CamInterface(char *libName);
                                    ~CamInterface(void);
    int                             getRevision();
    vector<string>        			getCameraStrings();
    version_info                    getVersion();
    int                             getNumberOfItems();
    bool                            open();
    bool                            close();
    bool                            setCurrentItem(int index);
    bool                            downloadItem(int index,BPath path,const char *name);
    bool                            deleteItem(int index);
    bool                            takeItem();
    char*                           getName();
    int                             getSize();
    char*                           getDate();
    BBitmap*                        getThumb();
    int                             getHeight();
    int                             getWidth();
    bool                            setCoreSystemLoop(BLooper *core);
    int                             getDevType();
    BWindow*                        pluginConfiguration(BPoint);
    bool                            hasFunction(pluginCaps function);
    std::vector<char*>              getLevel3Functions();
    bool                            executeLevel3Function(const char* name);

private:
    // Checks
    bool                            supportedFunctions[fns_count];
    // level 1
    status_t                        (*get_BDCP_API_Revision)();
    status_t                        (*getPluginVersion)(version_info&);
    status_t                        (*getSupportedCameras)(std::vector<std::string> &);
    status_t                        (*openCamera)();
    status_t                        (*closeCamera)();
    status_t                        (*getNumberofPics)(int &);
    status_t                        (*setCurrentPicture)(int);
    status_t                        (*downloadPicture)(BPath,const char*);
    // level 2
    status_t                        (*deletePicture)();
    status_t                        (*takePicture)();
    status_t                        (*getImageName)(char* &);
    status_t                        (*getImageSize)(int &);
    status_t                        (*getImageDate)(char* &);
    BWindow*                        (*configurePlugin)(BPoint);
    status_t                        (*setMessageTarget)(BLooper*);
    status_t                        (*getLevel3FunctionNames)(std::map<char*, char*>&);
    status_t                        (*getThumbnail)(BBitmap* &);
    // level 2 BDCP3
    status_t                        (*getImageHeight)(int &);
    status_t                        (*getImageWidth)(int &);
    status_t                        (*getDeviceType)(int &);
    // level 3
    // 		nothing yet
    //
    std::map<char*, level3Func>     level3Functions;
    bool                            getSymbols(image_id addonId);
    image_id                        addonId;
};

#define CAMI_OFFSET								655

#endif
