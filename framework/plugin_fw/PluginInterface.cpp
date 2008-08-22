/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//		Includes
#include <Path.h>
#include <FindDirectory.h>

// local includes

#include "PluginInterface.h"
#include "core_system.h"

//
//		External variables
extern class BeDiGiCamApp *app;

//		CamInterface::constructor
//
CamInterface::CamInterface(char *libName)
{
	//   initialization
	check_revision = false;
	check_pluginVersion = false;
	check_openCamera = false;
	check_closeCamera = false;
	check_numberOfPictures = false;
	check_currentPicture = false;
	check_downloadPicture = false;
	check_deletePicture = false;
	check_takePicture = false;
	check_imageName = false;
	check_imageSize = false;
	check_imageDate = false;
	check_configurePlugin = false;
	check_messageTarget = false;
	check_level3Functions = false;
	check_thumbnail = false;
	check_imageWidth = false;
	check_imageHeight = false;
	check_deviceType = false;
	check_supcams = false;
	//
	BEntry appentry;
	BPath path;
	// get the path of the application
	app_info info; 
  	app->GetAppInfo(&info); 
  	appentry.SetTo(&info.ref); 
    appentry.GetPath(&path);
	path.GetParent(&path);
	path.Append("plugins/");
	path.Append(libName);
	char test[B_FILE_NAME_LENGTH];
	strncpy(test,path.Path(),B_FILE_NAME_LENGTH);
	addonId = load_add_on(path.Path());
	camConnected = false;
	//
	if (addonId >= 0)
	{
		getSymbols(addonId);
	}
	else
	{
		logError(CAMI_LOAD_LIB);
	}
}
//		CamInterface::destructor
//
 CamInterface::~CamInterface()
{
	unload_add_on (addonId);
}
//
//	Interface::getSymbols
//
bool CamInterface::getSymbols(image_id pAddonId)
{
	if (get_image_symbol(pAddonId, "get_BDCP_API_Revision", B_SYMBOL_TYPE_TEXT, (void **)&get_BDCP_API_Revision) == B_OK)
	{
		check_revision = true;
	}
	if (get_image_symbol(pAddonId, "getPluginVersion", B_SYMBOL_TYPE_TEXT, (void **)&getPluginVersion) == B_OK)
	{
		check_pluginVersion = true;
	}
	if (get_image_symbol(pAddonId, "getSupportedCameras", B_SYMBOL_TYPE_TEXT, (void **)&getSupportedCameras) == B_OK)
	{
		check_supcams = true;
	}
	if (get_image_symbol(pAddonId, "openCamera", B_SYMBOL_TYPE_TEXT, (void **)&openCamera) == B_OK)
	{
		check_openCamera = true;
	}
	if (get_image_symbol(pAddonId, "closeCamera", B_SYMBOL_TYPE_TEXT, (void **)&closeCamera) == B_OK)
	{
		check_closeCamera = true;
	}
	if (get_image_symbol(pAddonId, "getNumberofPics", B_SYMBOL_TYPE_TEXT, (void **)&getNumberofPics) == B_OK)
	{
		check_numberOfPictures = true;
	}
	if (get_image_symbol(pAddonId, "setCurrentPicture", B_SYMBOL_TYPE_TEXT, (void **)&setCurrentPicture) == B_OK)
	{
		check_currentPicture = true;
	}
	if (get_image_symbol(pAddonId, "downloadPicture", B_SYMBOL_TYPE_TEXT, (void **)&downloadPicture) == B_OK)
	{
		check_downloadPicture = true;
	}
	if (get_image_symbol(pAddonId, "deletePicture", B_SYMBOL_TYPE_TEXT, (void **)&deletePicture) == B_OK)
	{
		check_deletePicture = true;
	}
	if (get_image_symbol(pAddonId, "takePicture", B_SYMBOL_TYPE_TEXT, (void **)&takePicture) == B_OK)
	{
		check_takePicture = true;
	}
	if (get_image_symbol(pAddonId, "getImageName", B_SYMBOL_TYPE_TEXT, (void **)&getImageName) == B_OK)
	{
		check_imageName = true;
	}
	if (get_image_symbol(pAddonId, "getImageSize", B_SYMBOL_TYPE_TEXT, (void **)&getImageSize) == B_OK)
	{
		check_imageSize = true;
	}
	if (get_image_symbol(pAddonId, "getImageDate", B_SYMBOL_TYPE_TEXT, (void **)&getImageDate) == B_OK)
	{
		check_imageDate = true;
	}
	if (get_image_symbol(pAddonId, "configurePlugin", B_SYMBOL_TYPE_TEXT, (void **)&configurePlugin) == B_OK)
	{
		check_configurePlugin = true;
	}
	if (get_image_symbol(pAddonId, "setMessageTarget", B_SYMBOL_TYPE_TEXT, (void **)&setMessageTarget) == B_OK)
	{
		check_messageTarget = true;
	}
	if (get_image_symbol(pAddonId, "getLevel3FunctionNames", B_SYMBOL_TYPE_TEXT, (void **)&getLevel3FunctionNames) == B_OK)
	{
		check_level3Functions = true;
	}
	if (get_image_symbol(pAddonId, "getThumbnail", B_SYMBOL_TYPE_TEXT, (void **)&getThumbnail) == B_OK)
	{
		check_thumbnail = true;
	}
	//Level 2 BDCP3
	if (get_image_symbol(pAddonId, "getImageHeight", B_SYMBOL_TYPE_TEXT, (void **)&getImageHeight) == B_OK)
	{
		check_imageHeight = true;
	}
	if (get_image_symbol(pAddonId, "getImageWidth", B_SYMBOL_TYPE_TEXT, (void **)&getImageWidth) == B_OK)
	{
		check_imageWidth = true;
	}
	if (get_image_symbol(pAddonId, "getDeviceType", B_SYMBOL_TYPE_TEXT, (void **)&getDeviceType) == B_OK)
	{
		check_deviceType = true;
	}
	return(true);
}
//
//		Interface: getRevision
int CamInterface::getRevision()
{
	if(check_revision)
		return (*get_BDCP_API_Revision)();
	return 0;
}
//
//		Interface: getCameraStrings
vector<string>	CamInterface::getCameraStrings()
{
	vector<string> supportedCams;
	if(check_supcams)
		(*getSupportedCameras)(supportedCams);
	return supportedCams;
}
//
//		Interface: openCamera
bool CamInterface::open()
{
	status_t err = B_ERROR;
	if (check_openCamera == true)
		err = (*openCamera)();
	
	if (err < B_OK)
	{
		camConnected = false;
		//unload_add_on(addonId);
		logError(CAMI_OPEN_CAMERA);
		return false;
	}
	else
		camConnected = true;
	return true;
}
//
//		Interface: closeCamera
bool CamInterface::close()
{
	camConnected = false;
	(*closeCamera)();
	return true;
}		
//
//		Interface: closeCamera
version_info CamInterface::getVersion()
{
	version_info pluginVersion;
	if(check_revision)
		(*getPluginVersion)(pluginVersion);
	return pluginVersion;
}
//
//		Interface: BDCP_logError
int CamInterface::getNumberOfItems()
{
	int numberOfItems = 0;
	if(check_numberOfPictures)
	{
		if((*getNumberofPics)(numberOfItems) == B_NO_ERROR)
			return numberOfItems;
	}	
	return 0;
}
//
//		Interface: BDCP_logError
bool CamInterface::setCurrentItem(int index)
{
	if(check_numberOfPictures)
	{
		(*setCurrentPicture)(index);
		return true;
	}
	return false;
}
//
//		Interface: Download picture
bool CamInterface::downloadItem(int index,BPath path)
{
	if(check_downloadPicture)
	{
		setCurrentItem(index);
		(*downloadPicture)(path);
		return true;
	}
	return false;
}
//
//		Interface: delete picture
bool CamInterface::deleteItem(int index)
{
	if(check_deletePicture)
	{
		setCurrentItem(index);
		(*deletePicture)();
		return true;
	}
	return false;
}
//
//		Interface: take picture
bool CamInterface::takeItem()
{
	if(check_takePicture)
	{
		(*takePicture)();
		return true;
	}
	return false;
}
//
//		Interface: get Name
char* CamInterface::getName()
{
	char * itemName;
	
	if(check_imageName)
	{
		(*getImageName)(itemName);
		return itemName;
	}
	return NULL;
}
//
//		Interface: get Size
int CamInterface::getSize()
{
	int itemSize;
	
	if(check_imageSize)
	{
		(*getImageSize)(itemSize);
		return itemSize;
	}
	return 0;
}
//
//		Interface: get Date
char* CamInterface::getDate()
{
	char * itemDate;
	
	if(check_imageDate)
	{
		(*getImageDate)(itemDate);
		return itemDate;
	}
	return NULL;
}
//
//		Interface: get Thumbnail
BBitmap* CamInterface::getThumb() 
{
	BBitmap* itemThumb;
	
	if(check_thumbnail)
	{
		(*getThumbnail)(itemThumb);
		return itemThumb;
	}
	return NULL;
}
//
//		Interface: get Height
int CamInterface::getHeight()
{
	int itemHeight;
	
	if(check_imageSize)
	{
		(*getImageHeight)(itemHeight);
		return itemHeight;
	}
	return 0;
}
//
//		Interface: get Width
int CamInterface::getWidth()
{
	int itemWidth;
	
	if(check_imageWidth)
	{
		(*getImageWidth)(itemWidth);
		return itemWidth;
	}
	return 0;
}
//
//		Interface: get the device type
int CamInterface::getDevType()
{
	int deviceType;
	
	if(check_deviceType)
	{
		(*getDeviceType)(deviceType);
		return deviceType;
	}
	return TYPE_PAR;
}
//
//		Interface: pass the core system loop the the plugin
bool CamInterface::setCoreSystemLoop(BLooper *core)
{
	if(check_messageTarget)
	{
		(*setMessageTarget)(core);
		return true;
	}
	return false;
}
//
//		Interface: camera Connection
bool CamInterface::cameraConnected() 
{
	return camConnected;
}
//
//		Interface: camera Connection
BWindow* CamInterface::pluginConfiguration(BPoint centerPoint) 
{
	if(check_configurePlugin)
		return (BWindow *)((*configurePlugin)(centerPoint));
	return NULL;
}
// 
//		Interface: BDCP_logError
int CamInterface::logError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case CAMI_OPEN_CAMERA:
			errorMessage = "BDCP: Could not open camera\n";
			break;
		case CAMI_LOAD_LIB:
			errorMessage = "BDCP: Could not load library\n";
			break;
		default:
			errorMessage = "BDCP: An unexpected error occured\n";
	}
	// write the errorMessage on the screen
	if(DEBUG_SCREEN || app->pgsettings.debugTerminal)
		printf(errorMessage);
	// write the errorMessage into the logfile
	if(DEBUG_LOGFILE || app->pgsettings.debugFile)
	{
		FILE	*file;
		
		file = fopen(LOGFILE,"a");
		fprintf(file,errorMessage);
		fclose(file);
	}
	return(ErrorMes);
}

//
//		Interface :: BDCP_LogValue
void CamInterface::logValue(int ValueMes, int Value)
{
	char 				*valueMessage;
	
	switch(ValueMes)
	{
		default:
			valueMessage = "BDCP: An unexpected error occured\n";
	}
	// write the value on the screen
	if(DEBUG_SCREEN || app->pgsettings.debugTerminal)
		printf(valueMessage,Value);
	// write the errorMessage into the logfile
	if(DEBUG_LOGFILE || app->pgsettings.debugFile)
	{
		FILE	*file;
		
		file = fopen(LOGFILE,"a");
		fprintf(file,valueMessage,Value);
		fclose(file);
	}
}
