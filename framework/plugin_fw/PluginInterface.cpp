/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//		Includes
using namespace std;

#include <Path.h>
#include <FindDirectory.h>
#include <iterator>

// local includes

#include "PluginInterface.h"
#include "core_system.h"
#include "logger.h"

//
//  CamInterface::constructor
//
CamInterface::CamInterface(char *libName) {
    //   initialization
    memset(supportedFunctions, 0, sizeof(supportedFunctions));

    //
    BEntry appentry;
    BPath path;
    // get the path of the application
    app_info info;
    be_app->GetAppInfo(&info);
    appentry.SetTo(&info.ref);
    appentry.GetPath(&path);
    path.GetParent(&path);
    path.Append("plugins/");
    path.Append(libName);
    addonId = load_add_on(path.Path());
    //
    if (addonId >= 0) {
	getSymbols(addonId);
    } else
    	LogDebug("CAMINTF - Plugin '%s' couldn't be loaded (%s).",libName, strerror(addonId));
}

//  CamInterface::destructor
//
CamInterface::~CamInterface() {
    unload_add_on (addonId);
}

//	Interface::getSymbols
//
bool
CamInterface::getSymbols(image_id pAddonId) {
    if (get_image_symbol(pAddonId, "get_BDCP_API_Revision", B_SYMBOL_TYPE_TEXT, (void **)&get_BDCP_API_Revision) == B_OK)
	    supportedFunctions[fn_revision] = true;
    if (get_image_symbol(pAddonId, "getPluginVersion", B_SYMBOL_TYPE_TEXT, (void **)&getPluginVersion) == B_OK)
	supportedFunctions[fn_pluginVersion] = true;
    if (get_image_symbol(pAddonId, "getSupportedCameras", B_SYMBOL_TYPE_TEXT, (void **)&getSupportedCameras) == B_OK)
	supportedFunctions[fn_supcams] = true;
    if (get_image_symbol(pAddonId, "openCamera", B_SYMBOL_TYPE_TEXT, (void **)&openCamera) == B_OK)
	supportedFunctions[fn_openCamera] = true;
    if (get_image_symbol(pAddonId, "closeCamera", B_SYMBOL_TYPE_TEXT, (void **)&closeCamera) == B_OK)
	supportedFunctions[fn_closeCamera] = true;
    if (get_image_symbol(pAddonId, "getNumberofPics", B_SYMBOL_TYPE_TEXT, (void **)&getNumberofPics) == B_OK)
	supportedFunctions[fn_numberOfPictures] = true;
    if (get_image_symbol(pAddonId, "setCurrentPicture", B_SYMBOL_TYPE_TEXT, (void **)&setCurrentPicture) == B_OK)
	supportedFunctions[fn_currentPicture] = true;
    if (get_image_symbol(pAddonId, "downloadPicture", B_SYMBOL_TYPE_TEXT, (void **)&downloadPicture) == B_OK)
	supportedFunctions[fn_downloadPicture] = true;
    if (get_image_symbol(pAddonId, "deletePicture", B_SYMBOL_TYPE_TEXT, (void **)&deletePicture) == B_OK)
	supportedFunctions[fn_deletePicture] = true;
    if (get_image_symbol(pAddonId, "takePicture", B_SYMBOL_TYPE_TEXT, (void **)&takePicture) == B_OK)
	supportedFunctions[fn_takePicture] = true;
    if (get_image_symbol(pAddonId, "getImageName", B_SYMBOL_TYPE_TEXT, (void **)&getImageName) == B_OK)
	supportedFunctions[fn_imageName] = true;
    if (get_image_symbol(pAddonId, "getImageSize", B_SYMBOL_TYPE_TEXT, (void **)&getImageSize) == B_OK)
	supportedFunctions[fn_imageSize] = true;
    if (get_image_symbol(pAddonId, "getImageDate", B_SYMBOL_TYPE_TEXT, (void **)&getImageDate) == B_OK)
	supportedFunctions[fn_imageDate] = true;
    if (get_image_symbol(pAddonId, "configurePlugin", B_SYMBOL_TYPE_TEXT, (void **)&configurePlugin) == B_OK)
	supportedFunctions[fn_configurePlugin] = true;
    if (get_image_symbol(pAddonId, "setMessageTarget", B_SYMBOL_TYPE_TEXT, (void **)&setMessageTarget) == B_OK)
	supportedFunctions[fn_messageTarget] = true;
    if (get_image_symbol(pAddonId, "getLevel3FunctionNames", B_SYMBOL_TYPE_TEXT, (void **)&getLevel3FunctionNames) == B_OK) {
	supportedFunctions[fn_level3Functions] = true;
	level3Functions.clear();
	map<char*, char*> l3f;
	status_t status = (*getLevel3FunctionNames)(l3f);
	if (!l3f.empty()) {
	    supportedFunctions[fn_level3Functions] = true;
	    for (map<char*, char*>::iterator i = l3f.begin(); i != l3f.end(); i++) {
		level3Func f;
		if (get_image_symbol(pAddonId, i->first, B_SYMBOL_TYPE_TEXT, (void**)&f) == B_OK) {
		    level3Functions[i->first] = f;
		}
	    }
	}
    }
    if (get_image_symbol(pAddonId, "getThumbnail", B_SYMBOL_TYPE_TEXT, (void **)&getThumbnail) == B_OK)
	supportedFunctions[fn_thumbnail] = true;
    //Level 2 BDCP3
    if (get_image_symbol(pAddonId, "getImageHeight", B_SYMBOL_TYPE_TEXT, (void **)&getImageHeight) == B_OK)
	supportedFunctions[fn_imageHeight] = true;
    if (get_image_symbol(pAddonId, "getImageWidth", B_SYMBOL_TYPE_TEXT, (void **)&getImageWidth) == B_OK)
	supportedFunctions[fn_imageWidth] = true;
    if (get_image_symbol(pAddonId, "getDeviceType", B_SYMBOL_TYPE_TEXT, (void **)&getDeviceType) == B_OK)
	supportedFunctions[fn_deviceType] = true;
    return(true);
}

bool
CamInterface::hasFunction(pluginCaps function) {
    if ((uint32)function <= fns_count)
        return supportedFunctions[function];
    else
        return false;
}
//
//		Interface: getRevision
int
CamInterface::getRevision() {
    if(supportedFunctions[fn_revision])
	return (*get_BDCP_API_Revision)();
    return 0;
}
//
//		Interface: getCameraStrings
vector<string>
CamInterface::getCameraStrings() {
    vector<string> supportedCams;
    if (supportedFunctions[fn_supcams])
	(*getSupportedCameras)(supportedCams);
    return supportedCams;
}
//
//		Interface: openCamera
bool
CamInterface::open() {
    status_t err = B_ERROR;
    if (supportedFunctions[fn_openCamera] == true)
	err = (*openCamera)();

    if (err != B_NO_ERROR) {
	LogDebug("CAMINTF - Couldn't open the camera.");
	return B_ERROR;
    }
    return B_OK;
}
//
//		Interface: closeCamera
bool
CamInterface::close() {
    status_t err = B_ERROR;
    if (supportedFunctions[fn_closeCamera] == true)
	err = (*closeCamera)();

    if (err != B_NO_ERROR) {
	LogDebug("CAMINTF - Couldn't close the camera.");
	return B_ERROR;
    }
    return B_OK;
}
//
//		Interface: closeCamera
version_info
CamInterface::getVersion() {
    version_info pluginVersion;
    if (supportedFunctions[fn_revision])
	(*getPluginVersion)(pluginVersion);
    return pluginVersion;
}
//
//		Interface: BDCP_logError
int
CamInterface::getNumberOfItems() {
    int numberOfItems = 0;
    if (supportedFunctions[fn_numberOfPictures]) {
        if((*getNumberofPics)(numberOfItems) == B_NO_ERROR)
            return numberOfItems;
    }
    return 0;
}
//
//		Interface: BDCP_logError
bool
CamInterface::setCurrentItem(int index) {
    if(supportedFunctions[fn_numberOfPictures]) {
	(*setCurrentPicture)(index);
	return B_OK;
    }
    return B_ERROR;
}
//
//		Interface: Download picture
bool
CamInterface::downloadItem(int index, BPath path, const char *name) {
    if (supportedFunctions[fn_downloadPicture]) {
	LogDebug("CAMINTF - File name is %s.", name);
	setCurrentItem(index);
	(*downloadPicture)(path,name);
	return B_OK;
    }
    return B_ERROR;
}
//
//		Interface: delete picture
bool
CamInterface::deleteItem(int index) {
    if(supportedFunctions[fn_deletePicture]) {
	setCurrentItem(index);
	(*deletePicture)();
	return B_OK;
    }
    return B_ERROR;
}
//
//  Interface: take picture
bool
CamInterface::takeItem() {
    if (supportedFunctions[fn_takePicture]) {
	(*takePicture)();
	return B_OK;
    }
    return B_ERROR;
}
//
//		Interface: get Name
char*
CamInterface::getName() {
    char * itemName;

    if (supportedFunctions[fn_imageName]) {
	(*getImageName)(itemName);
	return itemName;
    }
    return NULL;
}
//
//  Interface: get Size
int
CamInterface::getSize() {
    int itemSize;

    if(supportedFunctions[fn_imageSize]) {
	(*getImageSize)(itemSize);
	return itemSize;
    }
    return 0;
}
//
//  Interface: get Date
char*
CamInterface::getDate() {
    char* itemDate;

    if (supportedFunctions[fn_imageDate]) {
	(*getImageDate)(itemDate);
	return itemDate;
    }
    return NULL;
}
//
//		Interface: get Thumbnail
BBitmap*
CamInterface::getThumb() {
    BBitmap* itemThumb;

    if (supportedFunctions[fn_thumbnail]) {
	(*getThumbnail)(itemThumb);
	return itemThumb;
    }
    return NULL;
}
//
//		Interface: get Height
int
CamInterface::getHeight() {
    int itemHeight;

    if (supportedFunctions[fn_imageSize]) {
	(*getImageHeight)(itemHeight);
	return itemHeight;
    }
    return 0;
}
//
//		Interface: get Width
int
CamInterface::getWidth() {
    int itemWidth;

    if (supportedFunctions[fn_imageWidth]) {
	(*getImageWidth)(itemWidth);
	return itemWidth;
    }
    return 0;
}
//
//		Interface: get the device type
int
CamInterface::getDevType() {
    int deviceType;

    if (supportedFunctions[fn_deviceType]) {
	(*getDeviceType)(deviceType);
	return deviceType;
    }
    return TYPE_PAR;
}
//
//		Interface: pass the core system loop the the plugin
bool
CamInterface::setCoreSystemLoop(BLooper *core) {
    if (supportedFunctions[fn_messageTarget]) {
	(*setMessageTarget)(core);
	return true;
    }
    return false;
}
//
//		Interface: camera Connection
BWindow*
CamInterface::pluginConfiguration(BPoint centerPoint) {
    if (supportedFunctions[fn_configurePlugin])
		return (*configurePlugin)(centerPoint);
    return NULL;
}
