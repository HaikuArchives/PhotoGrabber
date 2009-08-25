/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef MSDINTF_H
#define MSDINTF_H

//
//		Local Includes
#include "global.h"
#include "MSDItem.h"
#include "scale.h"
//
//		Experimental Includes
#include <DiskDevice.h>
#include <DiskDevicePrivate.h>
#include <DiskDeviceRoster.h>
#include <DiskDeviceTypes.h>
#include <DiskDeviceList.h>
#include <Partition.h>

//
//		Includes
#include <image.h>
#include <AppFileInfo.h>
#include <map>
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <NodeInfo.h>
#include <Path.h>
#include <TranslationUtils.h>
#include <TranslatorRoster.h>
#include <TranslatorFormats.h>
#include <Picture.h>
#include <Bitmap.h>
#include <CAM.h>
#include <USBKit.h>
#include <fs_volume.h>
//
#define	THUMBHEIGHT					120
#define THUMBWIDTH					160
//
//		CameraInterface class
class MSDInterface
{
	public:
							MSDInterface(BUSBDevice *device);
							~MSDInterface(void);
			int 			MSD_logError(int ErrorMes);
			void			MSD_logValue(int ValueMes, int Value);
			//
			int				getNumberOfItems();
			bool			downloadItem(BPath path, const char *name);
			bool			deleteItem();
			char*			getName();
			uint32			getSize();
			uint32			getXRes();
			uint32			getYRes();
			char*			getDate();
			BBitmap*		getThumb();
			char*			getDeviceName();
			char*			getVersion();
			bool			cameraConnected();
			bool			IsMounted() const; 
			bool			Mount();
			bool			Unmount();
			bool			setCurrentItem(int index);

	private:
			bool					camConnected;
			char					*msdDeviceName;
			char					*msdVersion;
			char					*msdMountPoint;
			
			void					getMSDItems(const char *path);
			bool					supportedItem(BString mymetype);
			bool					removeMSDItem();
			MSDItem* 				getMSDItem();
			bool 					saveItem (MSDItem *item, const char *filename);
			std::map<uint32,MSDItem*> 	MSDItems;
			int						numberOfItems;
			BDiskDevice				*device;
			int 					currentItemHandle;
};

#define MS_OFFSET								1055
#define	MS_OPEN_CAMERA							MS_OFFSET + 1
#define	MS_LOAD_LIB								MS_OFFSET + 2
#define MSCAM_NO_HANDLES						MS_OFFSET + 3
#define	MS_CLOSE_CAMERA							MS_OFFSET + 4

#endif


