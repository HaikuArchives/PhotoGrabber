/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
// 
// File defenition
#ifndef CAMERA_H
#define CAMERA_H

 //	Includes
#include <SupportKit.h>
#include <Directory.h>
#include <Path.h>
#include <Looper.h>

//	Local Includes
#include "ItemData.h"
#include "PluginInterface.h"
#include "core_system.h"

//		define classes
//
class Camera  : public BLooper {
public:
								Camera(char *libName = NULL);
								~Camera();
	virtual void				MessageReceived(BMessage *message);
	bool						Start();
	bool						Stop();	
	bool						OpenDevice();
	bool						CloseDevice();
	bool						SetDownloadProps(BPath savedir);
	CamInterface				*camInterface;

private:
	bool						GetCameraItems();
	int							GetDeviceType();
	void						GetDeviceProperties();
	bool						GetDeviceCapability(pluginCaps capability);
	int							GetNumberOfItems();
	bool						DownloadItem(uint32 itemhandle,entry_ref *dir, const char *fileName);
	bool						RemoveItem(uint32 itemhandle);

	int							CameraNumberOfItems;
	BPath						CameraSavedir;
};

// Camera error messages

#define	CAM_OFFSET				155

#endif


