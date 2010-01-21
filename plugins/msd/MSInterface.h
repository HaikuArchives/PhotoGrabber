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
#include "Messages.h"
//
//		Experimental Includes
#include <DiskDevice.h>
#include <DiskDevicePrivate.h>
#include <DiskDeviceRoster.h>
#include <DiskDeviceTypes.h>
#include <DiskDeviceList.h>
#include <DiskDeviceVisitor.h>
#include <Partition.h>
#include <MountServer.h>

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
#include <Looper.h>
#include <fs_volume.h>
//
#define	THUMBHEIGHT					120
#define THUMBWIDTH					160
//
//		CameraInterface class
class MSInterface : public BLooper
{
	public:
							MSInterface(BLooper *mainLooper);
							~MSInterface();
			virtual void	MessageReceived(BMessage *message);
			
			bool			Start();
			bool			Stop();
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
			bool			setCurrentItem(int index);

	private:
			bool						_camConnected;
			char						*_msdDeviceName;
			char						*_msdVersion;
			BString				_mountPoint;
			std::map<uint32,MSDItem*> 	_MSDItems;
			int							_numberOfItems;
			BDiskDevice					*_device;
			int 						_currentItemHandle;
			MSDItem						*_currentItem;
			FILE						*_currentExifFile;
			unsigned char 				_currentBuffer[0x7fff];
			ssize_t 					_currentBufferSize;
			BLooper						*_mainLooper;
			
			status_t				_searchMountedVolumes();
			void					_getItems();
			void					_getMSDItems(const char *path);
			bool					_supportedItem(BString mymetype);
			bool					_removeMSDItem();
			MSDItem* 				_getMSDItem();
			bool 					_saveItem (MSDItem *item, const char *filename);
			void					_VolumeMounted(BMessage *message);
			void					_VolumeUnmounted(BMessage *message);
			int 					_logError(int ErrorMes);
			void					_logValue(int ValueMes, int Value);
			
};
//
// Visitor Class
class MSMountVisitor : public BDiskDeviceVisitor 
{
	public:
						MSMountVisitor();
		virtual			~MSMountVisitor();
		virtual bool 	Visit(BDiskDevice* device);
		virtual bool 	Visit(BPartition* partition, int32 level);
};

#define MS_OFFSET								1055
#define	MS_OPEN_CAMERA							MS_OFFSET + 1
#define	MS_LOAD_LIB								MS_OFFSET + 2
#define MSCAM_NO_HANDLES						MS_OFFSET + 3
#define	MS_CLOSE_CAMERA							MS_OFFSET + 4

#endif


