/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
//
// File defenition
#ifndef CAMINTF_H
#define CAMINTF_H

//		Includes
#include <map.h>
#include <Looper.h>
#include <Entry.h>
#include <Path.h>
#include <image.h>
#include <AppFileInfo.h>
#include <vector>
#include <string>

//		Local Includes
#include "core_global.h"

//		CameraInterface class
class CamInterface
{
	public:
						CamInterface(char *libName);
						~CamInterface(void);
		int 			logError(int ErrorMes);
		void			logValue(int ValueMes, int Value);
		int				getRevision();
		vector<string>	getCameraStrings();
		version_info	getVersion();
		int				getNumberOfItems();
		bool			open();
		bool			close();
		bool			setCurrentItem(int index);
		bool			downloadItem(int index,BPath path);
		bool			deleteItem(int index);
		bool			takeItem();
		char*			getName();
		int				getSize();
		char*			getDate();
		BBitmap*		getThumb();
		int				getHeight();
		int				getWidth();
		bool			setCoreSystemLoop(BLooper *core);
		int				getDevType();
		bool			cameraConnected(); 
		BWindow*		pluginConfiguration(BPoint);
		// Checks
		bool		check_revision;
		bool		check_supcams;
		bool		check_pluginVersion;
		bool		check_openCamera;
		bool		check_closeCamera;
		bool		check_numberOfPictures;
		bool		check_currentPicture;
		bool		check_downloadPicture;
		bool		check_deletePicture;
		bool		check_takePicture;
		bool		check_imageName;
		bool		check_imageSize;
		bool		check_imageDate;
		bool		check_configurePlugin;
		bool		check_messageTarget;
		bool		check_level3Functions;
		bool		check_thumbnail;
		bool		check_imageHeight;
		bool		check_imageWidth;
		bool		check_deviceType;

	private:
		// level 1
		status_t 	(*get_BDCP_API_Revision)();
		status_t 	(*getPluginVersion)(version_info &);
		status_t	(*getSupportedCameras)(vector<string> &);
		status_t 	(*openCamera)();
		status_t 	(*closeCamera)();
		status_t 	(*getNumberofPics)(int &);
		status_t 	(*setCurrentPicture)(int);
		status_t 	(*downloadPicture)(BPath);
		// level 2
		status_t 	(*deletePicture)();
		status_t 	(*takePicture)();
		status_t 	(*getImageName)(char* &);
		status_t 	(*getImageSize)(int &);
		status_t 	(*getImageDate)(char* &);
		status_t 	(*configurePlugin)(BPoint);
		status_t 	(*setMessageTarget)(BLooper*);
		status_t 	(*getLevel3FunctionNames)(map<char*, char*> &);
		status_t 	(*getThumbnail)(BBitmap* &);
		// level 2 BDCP3
		status_t 	(*getImageHeight)(int &);
		status_t 	(*getImageWidth)(int &);
		status_t 	(*getDeviceType)(int &);
		// level 3
		// 		nothing yet
		//
		bool		getSymbols(image_id addonId);
		image_id	addonId;
		// 
		bool		camConnected;
};

#define CAMI_OFFSET								655
#define	CAMI_OPEN_CAMERA						CAMI_OFFSET + 1
#define	CAMI_LOAD_LIB							CAMI_OFFSET + 2
#define CAMI_NO_HANDLES							CAMI_OFFSET + 3

#endif
