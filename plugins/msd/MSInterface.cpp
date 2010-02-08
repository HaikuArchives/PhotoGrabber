/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//		Local Includes
#include "MSInterface.h"
#include "global.h"
//#include "debug.h"
#include "logger.h"

//
// 		Includes
#include <Path.h>
#include <FindDirectory.h>
#include <View.h>
#include <Volume.h>
#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>

#include <stdlib.h>

//
// External variables
int READ_BUFFER  = 4096 * 4;
//
//		MSInterface constructor
MSInterface::MSInterface(BLooper *mainLooper) 
{	
	//   initialization
	_mainLooper = mainLooper;
	_msdDeviceName = new char[8];
	_msdVersion = new char[4];
	_camConnected = false;
	_numberOfItems = 0;
	_currentItemHandle = -1;
	_currentExifFile = NULL;
	if(_searchMountedVolumes() == B_OK)
	{
		_getItems();
		// Send a message to the system core
		BMessage *core_msg;
		core_msg = new BMessage(CAM_CONNECTED);
		core_msg->AddString("product",_mountPoint.String());
		if(_mainLooper != NULL)
		{
			LogDebug("MASS - Voume Mounted: Send message to the system core.");
			_mainLooper->PostMessage(core_msg);
		}
	}
	
}
//
//		MSInterface destructor
MSInterface::~MSInterface()
{	
	if(_currentExifFile != NULL)
		fclose(_currentExifFile);
	_currentExifFile = NULL;
	
}
//
//	MSInterface:: Start the mass storage interface looper
bool MSInterface::Start()
{
	LogDebug("MASS - Start listening to device list changes.");
	BLooper::Run();
	_searchMountedVolumes();
	BDiskDeviceRoster().StartWatching(this);
	return B_OK;
}
//
//	MSInterface:: Stop the mass storage interface looper
bool MSInterface::Stop()
{
	LogDebug("MASS - Stop listening to device list changes.");
	BLooper::Quit();
	BDiskDeviceRoster().StopWatching(this);
	return B_OK;
}
//
//  MSInterface:: Receive message
void MSInterface::MessageReceived(BMessage *message)
{
	
	switch(message->what)
	{
		case B_DEVICE_UPDATE:
		{
			LogDebug("MASS - Message Received :: Device update!");
			int32 event;
			if(message->FindInt32("event",&event) != B_OK)
				break;
			switch(event) 
			{
				LogDebug("MASS - Message Received :: There is an event '%d'!",event);
				case B_DEVICE_PARTITION_MOUNTED:
					_VolumeMounted(message);
					break;
				case B_DEVICE_PARTITION_UNMOUNTED:
					_VolumeUnmounted(message);
					break;	
				default:
					BLooper::MessageReceived(message);
			}
			break;
		}
		default: 
			BLooper::MessageReceived(message);
			break;
	}
}
//
//		MSInterface: MSD_logError
int MSInterface::getNumberOfItems()
{	
	LogDebug("MASS - GetThumb :: There are %d items.",_numberOfItems);
	return _numberOfItems;
}
//
//		MSInterface: setCurrentItem
bool MSInterface::setCurrentItem(int index)
{
	_currentItemHandle = index;
	_currentItem = _getMSDItem();
	if(_currentExifFile != NULL)
		fclose(_currentExifFile);
	_currentExifFile = fopen(_currentItem->ItemPath.String(), "rb");
	_currentBufferSize = fread(_currentBuffer, 1, sizeof(_currentBuffer), _currentExifFile);
	return true;
}
//
//		MSInterface: Download picture
bool MSInterface::downloadItem(BPath path,const char *name)
{
	char *filename;
	LogDebug("MASS - Download file: %s to %s.",_currentItem->ItemPath.String(),path.Path());
	int32 pathLength = strlen(path.Path());
	int32 fileNameLength = strlen(_currentItem->ItemName.String());
	off_t file_size = 0;
	status_t err;
	BFile *fh;
	int numberOfCopies = 1;
	while(numberOfCopies <= 100)
	{
		filename = new char[pathLength + fileNameLength + 3];
		strcpy(filename,path.Path());
		strcat(filename,"/");
		if(name != NULL)
			strcat(filename,name);
		else
			strcat(filename,_currentItem->ItemName.String());
		if(numberOfCopies > 1)
			sprintf(filename,"%s %d",filename,numberOfCopies);
		// Check if the file exists. If it exists, check if it is empty or not.
		fh = new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE);
		err = fh->GetSize(&file_size);
		if(err == B_OK && file_size == 0)
		{
			delete fh;
			if(_saveItem(_currentItem, filename) == B_NO_ERROR)
				return B_NO_ERROR;
			
		}
		else
			delete fh;
		numberOfCopies++;	
	}
	
	return B_ERROR;
}
//
//		MSInterface: delete picture
bool MSInterface::deleteItem()
{
	return _removeMSDItem();
}
//
//		MSInterface: get Name
char* MSInterface::getName()
{
	return (char *)_currentItem->ItemName.String();
}
//
//		MSInterface: get Size
uint32 MSInterface::getSize()
{
	BFile file;
	off_t size;
	status_t status = file.SetTo(_currentItem->ItemPath.String(),B_READ_ONLY);
    if(status == B_OK)
    {
		file.GetSize(&size);
		return (uint32)size;
    }
    return 0;
}
//
//		MSInterface: get X Resolution
uint32 MSInterface::getXRes()
{
	uint32 intValue = 0;
	if (_currentBufferSize != 0) 
	{
		ExifData *data = exif_data_new_from_data(_currentBuffer, _currentBufferSize);
		if (data)
		{ 
			// Get X resolution.
			char value[256];
			int count = 0;
			for (int i = 0; i < EXIF_IFD_COUNT; i++) 
			{
				if (i != 1 && data->ifd[i] && data->ifd[i]->count) 
				{
					ExifEntry* entry = exif_content_get_entry(data->ifd[i],EXIF_TAG_PIXEL_X_DIMENSION);
					if(entry)
					{
						exif_entry_get_value(entry, value, sizeof(value));
						intValue = atoi(value);
						LogDebug("MASS - GetXRes :: Resolution is %d.",intValue);
					}
				}
			}
			exif_data_unref(data);
		}
	}
	return intValue;
}
//
//		MSInterface: get Y Resolution
uint32 MSInterface::getYRes()
{
	uint32 intValue = 0;
	if (_currentBufferSize != 0) 
	{
		ExifData *data = exif_data_new_from_data(_currentBuffer, _currentBufferSize);
		if (data)
		{ 
			// Get X resolution.
			char value[256];
			int count = 0;
			for (int i = 0; i < EXIF_IFD_COUNT; i++) 
			{
				if (i != 1 && data->ifd[i] && data->ifd[i]->count) 
				{
					ExifEntry* entry = exif_content_get_entry(data->ifd[i],EXIF_TAG_PIXEL_Y_DIMENSION);
					if(entry)
					{
						exif_entry_get_value(entry, value, sizeof(value));
						intValue = atoi(value);
						LogDebug("MASS - GetYRes :: Resolution is %d.",intValue);
					}
				}
			}
			exif_data_unref(data);
		}
	}
	return intValue;
}
//
//		MSInterface: get Date
char* MSInterface::getDate()
{
	BFile file;
	off_t size;
	status_t status = file.SetTo(_currentItem->ItemPath.String(),B_READ_ONLY);
    if(status == B_OK)
    {
		time_t captureDate;
		file.GetCreationTime(&captureDate);
		struct tm *ptr;
		char* itemDate;
		itemDate = new char[10];
		ptr = gmtime(&captureDate);
		strftime(itemDate,100,"%d/%m/%Y",ptr);
		return itemDate;
    }
	return NULL;	
}
//
//		MSInterface: get Thumbnail
BBitmap* MSInterface::getThumb() 
{
	BBitmap *source = NULL;
	BBitmap *thumb = NULL;
	//	Get the image of the mass storage device
	// First check the bitmap in the EXIF data
	if (_currentBufferSize != 0) 
	{
		LogDebug("MASS - GetThumb :: Getting EXIF data.");
		ExifData *data = exif_data_new_from_data(_currentBuffer, _currentBufferSize);
		if (data)
		{ 
			LogDebug("MASS - GetThumb :: Get the EXIF image.");
			// Check EXIF image preview.
			if (data->data && data->size > 4) 
			{
				LogDebug("MASS - GetThumb :: EXIF thumbnail, %d bytes.",data->size);
				BMemoryIO in(data->data, data->size);
				thumb = BTranslationUtils::GetBitmap(&in);
			}
			exif_data_unref(data);
		}
	}
	if(thumb != NULL)
		return thumb;
	// Create thumb from image	
	source = BTranslationUtils::GetBitmapFile(_currentItem->ItemPath.String());
    if (source) 
    {
		LogDebug("MASS - GetThumb :: The source bitmap is present.");
		BRect dst(0, 0, THUMBWIDTH - 1, THUMBHEIGHT - 1);
		BRect src = source->Bounds();
		if (src.Width() > src.Height())
		  dst.bottom = dst.top + (src.Height() / src.Width() * dst.Width());
		else
		  dst.right = dst.left + (src.Width() / src.Height() * dst.Width());
  		thumb = new BBitmap(dst, source->ColorSpace());
		if (thumb) 
		{
			LogDebug("MASS - RescaleBitmap :: Begin scaling the new bitmap.");
			scale(source, thumb, dst.Width() / src.Width(), dst.Height() / src.Height());
		}
    	delete source;
    }
	return thumb;
}
//
//		MSInterface: get Device Name
char*	MSInterface::getDeviceName()
{
	return _msdDeviceName;
}
//
//		MSInterface: get Version
char*	MSInterface::getVersion()
{
	return _msdVersion;
}
//
//		MSInterface : save Item
bool MSInterface::_saveItem (MSDItem *item, const char *filename)
{
	BFile *fromfile = new BFile(item->ItemPath.String(), B_READ_ONLY);
	BFile *tofile;
	BNodeInfo *niTo, *niFrom;
	
	if((tofile = new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE )))
	{
		long int fherr;

		if((fherr = tofile->InitCheck()) != B_OK)
			return(B_ERROR);
		if((niTo = new BNodeInfo(tofile)))
		{
			if((niFrom = new BNodeInfo(fromfile)))
			{
				char *mymetype =  NULL;
				niFrom->GetType(mymetype);
				niTo->SetType(mymetype);
				delete niFrom;
			}
			else 
				niTo->SetType("image/jpeg");
			delete niTo;
		}
		// read the data from the fromfile and write it to the tofile
		char buf[READ_BUFFER];
		off_t size;
		fromfile->GetSize(&size);
		uint32 remainsize = (uint32)size;
		uint32 writesize = 0;
		while(remainsize > 0)
		{
			writesize = fromfile->Read((void *)buf,READ_BUFFER);
			if(( (fherr = tofile->Write(buf, writesize)) != writesize))
			{
				delete fromfile;
				delete tofile;
				return(B_ERROR);			
			}
			remainsize -= writesize;
		}
		delete fromfile;
		// Create the EXIF data as attributes
		FILE *exifFile = fopen(filename, "rb");
		unsigned char exifBuf[0x7fff];
		ssize_t bufsize = fread(exifBuf, 1, sizeof(buf), exifFile);
		if (bufsize == 0) 
		{
			fprintf (stderr, "Error reading file\n");
			fclose(exifFile);
			delete tofile;
			return (B_ERROR);
		}
		ExifData *data = exif_data_new_from_data(exifBuf, bufsize);
		if (!data)
		{ 
			fclose(exifFile);
			delete tofile;
			return (B_ERROR);
		}
		// Read EXIF tags.
		BString ident;
		char value[256];
		int count = 0;
		for (int i = 0; i < EXIF_IFD_COUNT; i++) 
		{
			if (i != 1 && data->ifd[i] && data->ifd[i]->count) 
			{
				for (unsigned int j = 0; j < data->ifd[i]->count; j++)
				{
					ExifEntry *e = data->ifd[i]->entries[j];
					exif_entry_get_value(e, value, sizeof(value));
					ident = "EXIF:";
					ident += exif_tag_get_name(e->tag);
					tofile->WriteAttr(ident.String(), B_STRING_TYPE, 0, (const void *)&value, strlen(value) + 1);
					count++;
				}
			}
		}
		exif_data_unref(data);
		fclose(exifFile);
		delete tofile;
	}
	return B_NO_ERROR;
}
//
//		MSInterface :: get the mass storage beginning from root
void MSInterface::_getItems()
{
	_getMSDItems(_mountPoint);
}
//
//		MSInterface :: get the mass storage files
void MSInterface::_getMSDItems(const char* path)
{
	BFile file;
	BNodeInfo fileInfo;
	char type[256];
	status_t status;
	LogDebug("MASS - GetMSDItems :: Searching in path  '%s'.", path);
	BDirectory dir(path);
	MSDItem *localItem;
  	if(dir.InitCheck() == B_OK)
  	{
    	BEntry entry;
    	while(dir.GetNextEntry(&entry,true) >= 0) 
    	{
      		BPath path;
      		entry.GetPath(&path);
      		if(entry.IsDirectory()) 
        		_getMSDItems(path.Path());
      		else if(entry.IsFile())
      		{
        		status = file.SetTo(path.Path(),B_READ_ONLY);
        		if(status == B_OK)
        		{
        			fileInfo.SetTo(&file);
        			if(fileInfo.GetType(type) == B_OK)
        			{
        				BString tmp;
        				tmp.Insert(type,256,0);
        				if(_supportedItem(tmp))
        				{
        					localItem = new MSDItem();
							// set the handle
							localItem->ItemHandle = _numberOfItems;
							// set the path
							localItem->ItemPath = path.Path();
							// set the file name
							localItem->ItemName = path.Leaf();
							// set the parent path
							path.GetParent(&path);
							localItem->ItemParent = path.Path();
							// Insert the Item in the list
							_MSDItems.insert(std::pair<uint32,MSDItem*>(_numberOfItems,localItem));
							_numberOfItems++;
        				}
        			}
        		}
      		}
      	}
    }
}
//
// 	MSInterface : Is the item supported
bool MSInterface::_supportedItem(BString mymetype)
{
	bool ret;
	
	if(mymetype.FindFirst("image/jpeg") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/jpeg") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("audio/aiff") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("audio/wav") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("audio/mpeg3") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("video/avi") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("video/mpeg") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("video/x-ms-asf") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("video/quicktime") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/tiff") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/vnd.fpx") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/bmp") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/gif") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/pcd") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/pict") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/png") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/jp2") != B_ERROR)
		ret = true;
	else if(mymetype.FindFirst("image/jpx") != B_ERROR)
		ret = true;
	else 
		ret = false;
				
	return ret;
}
//
//		MSInterface: camera Connection
bool MSInterface::cameraConnected() 
{
	return _camConnected;
}

//
// 		MSInterface : Mount
status_t MSInterface::_searchMountedVolumes()
{
		BPartition *partition = NULL;
		MSMountVisitor visitor;	
		BPath mountPoint;
		BDiskDeviceList devices;
		status_t status = devices.Fetch();
		if(status == B_OK)
		{
			partition = devices.VisitEachMountedPartition(&visitor);
			if(partition != NULL)
			{
				partition->GetMountPoint(&mountPoint);
				_mountPoint.SetTo(mountPoint.Path());
				return B_OK;
			}
		}
		return B_ERROR;
}
//
// 		MSInterface : Mount
void MSInterface::_VolumeMounted(BMessage *message)
{
        int32 id;
        if(message->FindInt32("id",&id) != B_OK)
        	return;
        BDiskDeviceRoster roster;
        BPartition *partition;
        BDiskDevice device;
        BPath mountPoint;
        
        if(roster.GetPartitionWithID(id,&device,&partition) != B_OK)
        	return;
        	
        partition->GetMountPoint(&mountPoint);
		LogDebug("MASS - Volume Mounted :: Found new mounted volume `%s' at '%s'.", partition->Name(), mountPoint.Path());
		_mountPoint.SetTo(mountPoint.Path());
		_getItems();
		// Send a message to the system core
		BMessage *core_msg;
		core_msg = new BMessage(CAM_CONNECTED);
		core_msg->AddString("product",_mountPoint.String());
		if(_mainLooper != NULL)
		{
			LogDebug("MASS - Voume Mounted: Send message to the system core.");
			_mainLooper->PostMessage(core_msg);
		}
}
//
// 		MSInterface : Unmount
void MSInterface::_VolumeUnmounted(BMessage *message)
{
        int32 id;
        if(message->FindInt32("id",&id) != B_OK)
        	return;
        BDiskDeviceRoster roster;
        BPartition *partition;
        BDiskDevice device;
        BPath mountPoint;
        
        if(roster.GetPartitionWithID(id,&device,&partition) != B_OK)
        	return;
        	
        partition->GetMountPoint(&mountPoint);

		if(_mountPoint.ICompare(mountPoint.Path()) == 0)
		{
			// send a message to the system core
			BMessage *core_msg;
			core_msg = new BMessage(CAM_DISCONNECTED);
			core_msg->AddString("product",mountPoint.Path());
			if(_mainLooper != NULL)
			{
				LogDebug("MASS - Volume Unmounted: Send message to the system core.");
				_mainLooper->PostMessage(core_msg);
			}
		}
}
//
//		MSInterface: get MSD Item
MSDItem* MSInterface::_getMSDItem()
{
	std::map<uint32,MSDItem*>::iterator i = _MSDItems.begin();
	while(i != _MSDItems.end())
	{
		if((*i).first == _currentItemHandle)
		{
			LogDebug("MASS - Get MSItem.");
			// return the MSD Item
			return (*i).second;
		}
		i++;
	}
	return NULL;
}
//
//		MSInterface: get MSD Item
bool MSInterface::_removeMSDItem()
{
	std::map<uint32,MSDItem*>::iterator i = _MSDItems.begin();
	while(i != _MSDItems.end())
	{
		if((*i).first == _currentItemHandle)
		{
			//
			printf("MSD - Delete Picture.");
			//TODO:: remove the item
			BEntry entry((*i).second->ItemPath.String());
			entry.Remove();
			(*i).second->ItemHandle = -1;
		}
		i++;
	}
	return true;
}
//
//VISITOR
MSMountVisitor::MSMountVisitor() 
{
	// Nothing yet
}
//
//
MSMountVisitor::~MSMountVisitor() {
	// Nothing yet
}
//
//
bool MSMountVisitor::Visit(BDiskDevice* device) { return false; }
//
//
bool MSMountVisitor::Visit(BPartition* partition, int32 level) 
{
	BPath mountPoint;
	partition->GetMountPoint(&mountPoint);
	// check if the mountPoint isn't boot
	BString searchString(mountPoint.Path());
	if(searchString.FindFirst("boot") == B_ERROR)
	{
		LogDebug("MASS - Search Mounted Volumes :: Found volume %s.", mountPoint.Path());
		return true;
	}
	return false;
} 

