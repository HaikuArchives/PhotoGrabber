/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/ 
// 
// File defenition
#ifndef CAMERA_H
#define CAMERA_H

 //	Includes
#include <SupportKit.h>
#include <Directory.h>
#include <Bitmap.h>
#include <DataIO.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
#include <Path.h>
#include <map.h>
#include <Looper.h>

//	Local Includes
#include "ItemData.h"
#include "PluginInterface.h"
#include "core_system.h"

//		define classes
//
class Camera  : public BLooper
{
	public:
						//Camera(char *name,char *manufacturer,char *version,char *serialnumber);
						Camera(char *libName,char *looperName);
						~Camera();
		virtual void	MessageReceived(BMessage *message);
		virtual bool	QuitRequested();	
		bool			OpenCamera();
		bool			CloseCamera();
		bool			CheckCameraConnection();
		const char* 	GetModel();
		const char* 	GetManufacturer();
		const char* 	GetVersion();
		const char* 	GetSerialNumber();
		int	 			GetNumberOfItems();
		bool			DownloadItem(uint32 itemhandle,entry_ref *dir);
		bool			RemoveItem(uint32 itemhandle);
		bool			GetStorageInfo(void);
		void 			SetModel(char *model);
		void 			SetManufacturer(char *manufac);
		void 			SetVersion(char *version);
		void 			SetSerialNumber(char *serialnumber);
		bool			SetDownloadProps(BPath savedir);
		bool			GetCameraItems();
		int				GetDeviceType();
		bool			IsOpen();
		CamInterface	*camInterface;

	protected:
		BString					CameraModel;
		BString					CameraManufacturer;
		BString					CameraVersion;
		BString					CameraSerialNumber;
		int						CameraNumberOfItems;
		BPath 					CameraSavedir;
		bool					looperRunning;
		// Functions
		bool			GetCameraInfo();
		bool			SaveCameraItem(char *data, long int size, const char filename[255]);
		BBitmap*		GetItemThumbnail(uint32 index);
		int				logCamError(int ErrorMes);

};

// Camera error messages

#define	CAM_OFFSET				155
#define CAM_DEV_NO_FIND			CAM_OFFSET + 1
#define CAM_INIT_FAIL			CAM_OFFSET + 2
#define CAM_NO_HANDLES			CAM_OFFSET + 3
#define CAM_GETPIC_FAIL			CAM_OFFSET + 4
#define CAM_FILE_EXISTS			CAM_OFFSET + 5
#define CAM_FILECREATE_FAIL		CAM_OFFSET + 6
#define CAM_VOLUME_FULL			CAM_OFFSET + 7
#define CAM_FILEWRITE_FAIL		CAM_OFFSET + 8
#define CAM_TRANSFER_FAIL		CAM_OFFSET + 9

#endif


