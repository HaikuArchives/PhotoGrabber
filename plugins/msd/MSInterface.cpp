/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//		Local Includes
#include "MSInterface.h"
#include "global.h"
#include "debug.h"

//
// 		Includes
#include <Path.h>
#include <FindDirectory.h>
#include <View.h>
#include <Volume.h>
#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>

//
// External variables
int READ_BUFFER  = 1024;
//
//		MSDInterface constructor
MSDInterface::MSDInterface(BUSBDevice *dev) 
{	
	//   initialization
	msdDeviceName = new char[8];
	msdVersion = new char[4];
	msdMountPoint = new char[B_FILE_NAME_LENGTH];
	camConnected = false;
	numberOfItems = 0;
	currentItemHandle = -1;
}
MSDInterface::~MSDInterface()
{	
	// Nothing yet
}
//
//		MSDInterface: MSD_logError
int MSDInterface::getNumberOfItems()
{	
	return numberOfItems;
}
//
//		MSDInterface: setCurrentItem
bool MSDInterface::setCurrentItem(int index)
{
	currentItemHandle = index;
	return true;
}
//
//		MSDInterface: Download picture
bool MSDInterface::downloadItem(BPath path,const char *name)
{
	MSDItem *item;
	item = getMSDItem();
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,"MS - Download file: %s to %s\n",item->ItemPath.String(),path.Path());
		fclose(file);
	#endif
	BFile *fromfile = new BFile(item->ItemPath.String(), B_READ_ONLY);
	BFile *tofile;
	BNodeInfo *ni;
	#ifdef DEBUG
		file = fopen(LOGFILE,"a");
		fprintf(file,"MS - Create To-filename.\n");
		fclose(file);
	#endif
	char filename[B_FILE_NAME_LENGTH];
	strcpy(filename,path.Path());
	strcat(filename,"/");
	strcat(filename,item->ItemName.String());
	#ifdef DEBUG
		file = fopen(LOGFILE,"a");
		fprintf(file,"MS - To-filename created.\n");
		fclose(file);
	#endif 
	if((tofile=new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE )))
	{
		long int fherr;

		if((fherr = tofile->InitCheck()) != B_OK)
			return(B_ERROR);
		if((ni=new BNodeInfo(tofile)))
		{
			ni->SetType("image/jpeg");
			delete ni;
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
			fclose(file);
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
	return B_OK;
}
//
//		MSDInterface: delete picture
bool MSDInterface::deleteItem()
{
	return removeMSDItem();
}
//
//		MSDInterface: get Name
char* MSDInterface::getName()
{
	MSDItem *item;
	item = getMSDItem();
	return (char *)item->ItemName.String();
}
//
//		MSDInterface: get Size
uint32 MSDInterface::getSize()
{
	MSDItem *item;
	item = getMSDItem();
	//
	BFile file;
	off_t size;
	status_t status = file.SetTo(item->ItemPath.String(),B_READ_ONLY);
    if(status == B_OK)
    {
		file.GetSize(&size);
		return (uint32)size;
    }
    return 0;
}
//
//		MSDInterface: get X Res
uint32 MSDInterface::getXRes()
{
	MSDItem *item;
	item = getMSDItem();
	uint32 intValue = 0;
	FILE *exifFile = fopen(item->ItemPath.String(), "rb");
	unsigned char buf[0x7fff];
	ssize_t bufsize = fread(buf, 1, sizeof(buf), exifFile);
	if (bufsize != 0) 
	{
		ExifData *data = exif_data_new_from_data(buf, bufsize);
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
						#ifdef DEBUG
							FILE *file = fopen(LOGFILE,"a");
							fprintf(file,"MS - GetXRes :: Resolution is %d.\n",intValue);
							fclose(file);
						#endif
					}
				}
			}
			exif_data_unref(data);	
			fclose(exifFile);
		}
	}
	return intValue;
}
//
//		MSDInterface: get Y Res
uint32 MSDInterface::getYRes()
{
	MSDItem *item;
	item = getMSDItem();
	uint32 intValue = 0;
	FILE *exifFile = fopen(item->ItemPath.String(), "rb");
	unsigned char buf[0x7fff];
	ssize_t bufsize = fread(buf, 1, sizeof(buf), exifFile);
	if (bufsize != 0) 
	{
		ExifData *data = exif_data_new_from_data(buf, bufsize);
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
						#ifdef DEBUG
							FILE *file = fopen(LOGFILE,"a");
							fprintf(file,"MS - GetYRes :: Resolution is %d.\n",intValue);
							fclose(file);
						#endif
					}
				}
			}
			exif_data_unref(data);	
			fclose(exifFile);
		}
	}
	return intValue;
}
//
//		MSDInterface: get Date
char* MSDInterface::getDate()
{
	MSDItem *item;
	item = getMSDItem();
	BFile file;
	off_t size;
	status_t status = file.SetTo(item->ItemPath.String(),B_READ_ONLY);
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
//		MSDInterface: get Thumbnail
BBitmap* MSDInterface::getThumb() 
{
	BBitmap *source = NULL;
	BBitmap *thumb = NULL;
	//	Get the image of the mass storage device
	MSDItem *item;
	item = getMSDItem();
	// First check the bitmap in the EXIF data
	FILE *file = fopen(item->ItemPath.String(), "rb");
	unsigned char buf[0x7fff];
	ssize_t bufsize = fread(buf, 1, sizeof(buf), file);
	if (bufsize != 0) 
	{
		#ifdef DEBUG
			FILE	*file;
			file = fopen(LOGFILE,"a");
			fprintf(file,"MS - GetThumb :: Getting EXIF data.\n");
			fclose(file);
		#endif
		ExifData *data = exif_data_new_from_data(buf, bufsize);
		if (data)
		{ 
			#ifdef DEBUG
				file = fopen(LOGFILE,"a");
				fprintf(file,"MS - GetThumb :: Get the EXIF image.\n");
				fclose(file);
			#endif
			// Check EXIF image preview.
			if (data->data && data->size > 4) 
			{
				#ifdef DEBUG
					file = fopen(LOGFILE,"a");
					fprintf(file,"MS - GetThumb :: EXIF thumbnail, %d bytes.\n",data->size);
					fclose(file);
				#endif
				BMemoryIO in(data->data, data->size);
				thumb = BTranslationUtils::GetBitmap(&in);
			}
			exif_data_unref(data);
		}
	}
	fclose(file);
	if(thumb != NULL)
		return thumb;
	// Create thumb from image	
	source = BTranslationUtils::GetBitmapFile(item->ItemPath.String());
    if (source) 
    {
		#ifdef DEBUG
			file = fopen(LOGFILE,"a");
			fprintf(file,"MS - GetThumb :: The source bitmap is present.\n");
			fclose(file);
		#endif
		BRect dst(0, 0, THUMBWIDTH - 1, THUMBHEIGHT - 1);
		BRect src = source->Bounds();
		if (src.Width() > src.Height())
		  dst.bottom = dst.top + (src.Height() / src.Width() * dst.Width());
		else
		  dst.right = dst.left + (src.Width() / src.Height() * dst.Width());
  		thumb = new BBitmap(dst, source->ColorSpace());
		if (thumb) 
		{
			#ifdef DEBUG
				file = fopen(LOGFILE,"a");
				fprintf(file,"MS - RescaleBitmap :: Begin scaling the new bitmap.\n");
				fclose(file);
			#endif
			scale(source, thumb, dst.Width() / src.Width(), dst.Height() / src.Height());
		}
    	delete source;
    }
	return thumb;
}
//
//		MSDInterface: get Device Name
char*	MSDInterface::getDeviceName()
{
	return msdDeviceName;
}
//
//		MSDInterface: get Version
char*	MSDInterface::getVersion()
{
	return msdVersion;
}
//
//
void MSDInterface::getMSDItems(const char* path)
{
	// TODO :: 	Surfing in the file structure of the mass storage device
	//			and create a MSD_Item when we find a picture.
	BFile file;
	BNodeInfo fileInfo;
	char type[256];
	status_t status;
	#ifdef DEBUG
		FILE	*logFile;
		logFile = fopen(LOGFILE,"a");
		fprintf(logFile,"MS - GetMSDItems :: Searching in path  '%s'.\n", path);
		fclose(logFile);
	#endif
	BDirectory dir(path);
	MSDItem *localItem;
  	if(dir.InitCheck() == B_OK)
  	{
    	BEntry entry;
    	while(dir.GetNextEntry(&entry,true) >= 0) 
    	{
      		BPath name;
      		entry.GetPath(&name);
      		if(entry.IsDirectory()) 
        		getMSDItems(name.Path());
      		else if(entry.IsFile())
      		{
        		status = file.SetTo(name.Path(),B_READ_ONLY);
        		if(status == B_OK)
        		{
        			fileInfo.SetTo(&file);
        			if(fileInfo.GetType(type) == B_OK)
        			{
        				BString tmp;
        				tmp.Insert(type,256,0);
        				if(tmp.FindFirst("image/jpeg") > B_ERROR)
        				{
        					localItem = new MSDItem();
							// set the handle
							localItem->ItemHandle = numberOfItems;
							// set the path
							localItem->ItemPath = name.Path();
							// set the file name
							localItem->ItemName = name.Leaf();
							// Insert the Item in the list
							MSDItems.insert(pair<uint32,MSDItem*>(numberOfItems,localItem));
							numberOfItems++;
        				}
        			}
        		}
      		}
      	}
    }
}
//
//		MSDInterface: camera Connection
bool MSDInterface::cameraConnected() 
{
	return camConnected;
}
//
//		MSDInterface : IsMounted()
bool
MSDInterface::IsMounted() const
{
        return device->IsMounted();
}
//
// 	MSDInterface : Mount
bool MSDInterface::Mount()
{
		char *imgDir = "DCIM";
		BDiskDeviceList deviceList;
		status_t error = deviceList.Fetch();
		if (error != B_OK) 
		{
			#ifdef DEBUG
				FILE	*file;
				file = fopen(LOGFILE,"a");
				fprintf(file,"MS - Mount :: Failed to get the list of disk devices: %s\n", strerror(error));
				fclose(file);
			#endif
			return B_ERROR;
		}
		#ifdef DEBUG
			FILE	*file;
			file = fopen(LOGFILE,"a");
			fprintf(file,"MS - Mount :: There are %d disk devices.\n", deviceList.CountDevices());
			fclose(file);
		#endif
		for(int i = 0; i < deviceList.CountDevices();i++)
		{
			BDiskDevice *device = deviceList.DeviceAt(i);
			#ifdef DEBUG
					FILE	*file;
					file = fopen(LOGFILE,"a");
					fprintf(file,"MS - Mount :: Device %d:\n", i + 1);
					fprintf(file,"MS - Mount :: Name: %s.\n", device->Name());
					BPath path;
					device->GetPath(&path);
					fprintf(file,"MS - Mount :: Path:%s.\n\n", path.Path());
					fclose(file);
			#endif
			int32 usbPresent = strlen("/dev/disk/usb/");
			if(device && device->IsRemovableMedia()  
					&& !strncmp(path.Path(), "/dev/disk/usb/", usbPresent))
			{
				#ifdef DEBUG
					FILE	*file;
					file = fopen(LOGFILE,"a");
					fprintf(file,"MS - Mount :: There are %d partitions on device %d.\n", device->CountDescendants(), i + 1);
					fclose(file);
				#endif
				BPath mountPoint;
				// Mount device when there is only 1 partition
				if(device->CountDescendants() == 1)
				{
					if(device->IsMounted())
					{
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Device already mounted. Continue getting items.\n");
							fclose(file);
						#endif
						device->GetMountPoint(&mountPoint);
						mountPoint.Append(imgDir);
						getMSDItems(mountPoint.Path());
						return B_OK;
					}		
					status_t error = device->Mount(NULL,B_MOUNT_READ_ONLY);
					if (error >= B_OK) 
					{
						device->GetMountPoint(&mountPoint);
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Volume `%s' mounted successfully at '%s'.\n", device->Name(), mountPoint.Path());
							fclose(file);
						#endif
						// Get the items
						mountPoint.Append(imgDir);
						getMSDItems(mountPoint.Path());
						return B_OK;
					} 
					else 
					{	
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Volume `%s' mounted failed!.\n", device->Name());
							fclose(file);
						#endif
						return B_ERROR;
					}
				}
				// Mount the right partition when there are more then 1
				for(int j = 0; j < device->CountDescendants(); j++)
				{
					BPartition *partition = device->ChildAt(j);
					if(partition->IsMounted())
					{
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Partition already mounted. Continue getting items.\n");
							fclose(file);
						#endif
						BPath mountPoint;
						partition->GetMountPoint(&mountPoint);
						mountPoint.Append(imgDir);
						getMSDItems(mountPoint.Path());
						return B_OK;
					}
					status_t error = partition->Mount(NULL,B_MOUNT_READ_ONLY);
					if (error >= B_OK) 
					{
						partition->GetMountPoint(&mountPoint);
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Volume `%s' mounted successfully at '%s'.\n", partition->Name(), mountPoint.Path());
							fclose(file);
						#endif
						// Get the items
						mountPoint.Append(imgDir);
						getMSDItems(mountPoint.Path());
						return B_OK;
					} 
					else 
					{	
						#ifdef DEBUG
							FILE	*file;
							file = fopen(LOGFILE,"a");
							fprintf(file,"MS - Mount :: Volume `%s' mounted failed!.\n", partition->Name());
							fclose(file);
						#endif
					}
				}
			}
		}
		return B_ERROR;
}
//
// 	MSDInterface : Unmount
bool MSDInterface::Unmount()
{
        return false;
}
//
//		MSDInterface: get MSD Item
MSDItem* MSDInterface::getMSDItem()
{
	map<uint32,MSDItem*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == currentItemHandle)
		{
			#ifdef DEBUG
				FILE	*file;
				file = fopen(LOGFILE,"a");
				fprintf(file,"MS: Get MSItem\n");
				fclose(file);
			#endif
			// return the MSD Item
			return (*i).second;
		}
		i++;
	}
	return NULL;
}
//
//		MSDInterface: get MSD Item
bool MSDInterface::removeMSDItem()
{
	map<uint32,MSDItem*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == currentItemHandle)
		{
			//
			printf("MSD - Delete Picture\n");
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
//		MSDInterface: MSD_logError
int MSDInterface::MSD_logError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case MS_OPEN_CAMERA:
			errorMessage = "MS: Could not mount the scsi device\n";
			break;
		case MS_CLOSE_CAMERA:
			errorMessage = "MS: Could not unmount the scsi device\n";
			break;
		default:
			errorMessage = "MS: An unexpected error occured\n";
	}
	// write the errorMessage into the logfile
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,errorMessage);
		fclose(file);
	#endif
	return(ErrorMes);
}
//
//		MSDInterface :: MSD_LogValue
void MSDInterface::MSD_logValue(int ValueMes, int Value)
{
	char 				*valueMessage;
	
	switch(ValueMes)
	{
		default:
			valueMessage = "MS: An unexpected error occured\n";
	}
	// write the errorMessage into the logfile
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,valueMessage,Value);
		fclose(file);
	#endif
}
