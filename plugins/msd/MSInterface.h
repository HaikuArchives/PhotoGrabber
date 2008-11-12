/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/

//
// File defenition
#ifndef MSDINTF_H
#define MSDINTF_H

//
//		Local Includes
#include "global.h"
#include "MSDItem.h"
#include "BitMapView.h"

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

struct user_partition_data
{
	bool mounted;
};

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
			bool			downloadItem(int index,BPath path);
			bool			deleteItem(int index);
			char*			getName(int index);
			int				getSize(int index);
			int				getXRes(int index);
			int				getYRes(int index);
			char*			getDate(int index);
			BBitmap*		getThumb(int index);
			char *			getDeviceName();
			char*			getVersion();
			bool			cameraConnected();
			bool			IsMounted() const; 
			status_t		GetMountPoint(BPath* mountPoint) const;
			dev_t			Mount(const char* mountPoint, uint32 mountFlags,const char* parameters);
			status_t		Unmount(uint32 unmountFlags);

	private:
			bool					camConnected;
			char					*msdDeviceName;
			char					*msdVersion;
			char					*msdMountPoint;
			bool					setCurrentItem(int index);
			void					getMSDItems(const char *path);
			bool					removeMSDItem(int handle);
			MSD_Item* 				getMSDItem(int handle);
			void					RescaleBitmap(char *srcfilename, BBitmap *dest);
			map<uint32,MSD_Item*> 	MSDItems;
			int						numberOfItems;
			user_partition_data		msdPartitionData;
};

#define MS_OFFSET								1055
#define	MS_OPEN_CAMERA							MS_OFFSET + 1
#define	MS_LOAD_LIB								MS_OFFSET + 2
#define MSCAM_NO_HANDLES						MS_OFFSET + 3
#define	MS_CLOSE_CAMERA							MS_OFFSET + 4

#endif


