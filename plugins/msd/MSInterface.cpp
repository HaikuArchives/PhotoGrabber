/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
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
#ifdef _ZETA_OS_
	#include <experimental/BitmapTools.h>
#endif

//
// External variables
//extern class BeDiGiCamApp *app;
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
	SCSIMount("/dev/disk/scsi",dev);
}
MSDInterface::~MSDInterface()
{	
	SCSIUnmount();
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
	return false;
}
//
//		MSDInterface: Download picture
bool MSDInterface::downloadItem(int index,BPath path)
{
	MSD_Item *item;
	item = getMSDItem(index);
	#ifdef DEBUG
	FILE	*file;
	file = fopen(LOGFILE,"a");
	fprintf(file,"Donwload file: %s to %s\n",item->GetItemPath(),path.Path());
	fclose(file);
	#endif
	BFile *fromfile = new BFile(item->GetItemPath(), B_READ_ONLY);
	BFile *tofile;
	BNodeInfo *ni;
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"Create tofilename");
	fclose(file);
	#endif
	char filename[B_FILE_NAME_LENGTH];
	strcpy(filename,path.Path());
	strcat(filename,"/");
	strcat(filename,item->GetName());
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"tofilename created");
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
		int32 size = item->GetSize();
		int32 remainsize = size;
		int32 writesize = 0;
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
		delete tofile;
	}
	return true;
}
//
//		MSDInterface: delete picture
bool MSDInterface::deleteItem(int index)
{
	return removeMSDItem(index);
}
//
//		MSDInterface: get Name
char* MSDInterface::getName(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return (char *)item->GetName();
}
//
//		MSDInterface: get Size
int MSDInterface::getSize(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetSize();
}
//
//		MSDInterface: get X Res
int MSDInterface::getXRes(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetXRes();
}
//
//		MSDInterface: get Y Res
int MSDInterface::getYRes(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetYRes();
}
//
//		MSDInterface: get Date
char* MSDInterface::getDate(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return (char *)item->GetDate();
}
//
//		MSDInterface: get Thumbnail
BBitmap* MSDInterface::getThumb(int index) 
{
	BBitmap *source;
	BBitmap *thumb;
	//	Get the image of the scsi device
	MSD_Item *item;
	item = getMSDItem(index);
	#ifdef DEBUG
	FILE	*file;
	file = fopen(LOGFILE,"a");
	fprintf(file,"This is the item file: %s\n",item->GetItemPath());
	fclose(file);
	#endif
	source = BTranslationUtils::GetBitmapFile(item->GetItemPath());
	BRect rect(0,0,150,113);
	thumb = new BBitmap(rect,B_RGB32);
	//	Rescale the bitmap
	#ifdef _ZETA_OS_
		scale_bitmap(thumb,source);
	#else
		// Should be made fore Haiku
		//RescaleBitmap((char *)item->GetItemPath(),thumb);
		//
		//No deletion of the item pointer!!!! delete(item);
		//
		//delete(source);
	#endif
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
void MSDInterface::getMSDItems(const char *path)
{
	// TODO :: 	Surfing in the file structure of the mass storage device
	//			and create a MSD_Item when we find a picture.
	BFile file;
	BNodeInfo fileInfo;
	char type[256];
	status_t status;
	BDirectory dir(path);
	MSD_Item *localItem;
	//int i =0;
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
        					localItem = new MSD_Item();
							// set the handle
							localItem->setHandle(numberOfItems);
							// set the path
							localItem->setItemPath(name.Path());
							off_t size;
							file.GetSize(&size);
							localItem->setSize((uint32)size);
							// set the name
							localItem->setName(name.Leaf());
							// set the resolution
							BBitmap *image;
							BTranslatorRoster *proster = BTranslatorRoster::Default();
							if (!proster)
							{
								printf("MSD - TranslationRoster problem\n");
								return;
							}
							translator_info info;
							memset(&info, 0, sizeof(translator_info));
							BMessage ioExtension;
							int ret = proster->Identify(&file, &ioExtension, &info, 0, NULL,B_TRANSLATOR_BITMAP);
							if(ret == B_NO_TRANSLATOR)
							{
								printf("MSD - No Translator found\n");
								return;
							}
							else if(ret == B_NOT_INITIALIZED)
							{
								printf("MSD - Not Initialized found\n");
								return;	
							}
							image = BTranslationUtils::GetBitmapFile(name.Path(),proster);
							BRect bounds;
							bounds = image->Bounds();
							localItem->setRes(bounds.Width(),bounds.Height());
							delete(image);
							// set the capture date
							time_t captureDate;
							file.GetCreationTime(&captureDate);
							struct tm *ptr;
							char *tmpdate;
							tmpdate = new char[10];
							ptr = gmtime(&captureDate);
							strftime(tmpdate,100,"%d/%m/%Y",ptr);
							localItem->setDate(tmpdate);
							// Insert the Item in the list
							MSDItems.insert(pair<int,MSD_Item*>(numberOfItems,localItem));
							numberOfItems++;
        				}
        			}
        		}
      		}
      	}
    }
   //numberOfItems = i +1;
}
//
//		MSDInterface: camera Connection
bool MSDInterface::cameraConnected() 
{
	return camConnected;
}
//
//		MSDInterface: get MSD Item
MSD_Item* MSDInterface::getMSDItem(int handle)
{
	map<uint32,MSD_Item*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == handle)
		{
			//
			#ifdef DEBUG
			FILE	*file;
			file = fopen(LOGFILE,"a");
			fprintf(file,"Get MSItem\n");
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
bool MSDInterface::removeMSDItem(int handle)
{
	map<uint32,MSD_Item*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == handle)
		{
			//
			printf("MSD - Delete Picture\n");
			//TODO:: remove the item
			BEntry entry((*i).second->GetItemPath());
			entry.Remove();
			(*i).second->setHandle(-1);
		}
		// set the wright handles to the items 
		// behind the deleted item
		//else if((int)(*i).second->GetHandle() > handle && (int)(*i).second->GetHandle() != -1)
		//		(*i).second->setHandle((*i).second->GetHandle()-1);
		i++;
	}
	return true;
}
//
// MSDInterface :: Probe and mount the correct SCSI device
bool MSDInterface::SCSIProbeMount(const char *device_path, BUSBDevice *usbdev)
{
  int fd,e;
  int path,targ,lun;
  raw_device_command rdc;
  uchar data[36], sense[16];
  //char 	volume_name[B_FILE_NAME_LENGTH];
  char	mount_point[B_FILE_NAME_LENGTH];
  char	tmp[B_FILE_NAME_LENGTH];
  int	index = 1;
  BDirectory directory;

  if(!strncmp("/dev/disk/scsi/",device_path,14))
    sscanf(device_path,"/dev/disk/scsi/%d/%d/%d/raw",&path,&targ,&lun); 
  else 
    return false;
  // fill out our raw device command data
  rdc.data = data;
  rdc.data_length = 36;
  rdc.sense_data = sense;
  rdc.sense_data_length = 0;
  rdc.timeout = 1000000;
  rdc.flags = B_RAW_DEVICE_DATA_IN;
  rdc.command_length = 6;
  rdc.command[0] = 0x12;
  rdc.command[1] = 0x00;
  rdc.command[2] = 0x00;
  rdc.command[3] = 0x00;
  rdc.command[4] = 36;
  rdc.command[5] = 0x00;
  // Open the raw device
  if((fd = open(device_path,0)) < 0) 
  	return false;
  e = ioctl(fd, B_RAW_DEVICE_COMMAND, &rdc, sizeof(rdc));
  close(fd);
  if((e != 0) || (rdc.cam_status != CAM_REQ_CMP)) 
  	return false;
  // Check if the Manufacturer string of the usb device is the same as the 
  // one of the scsi device. If so, try to mount the device. Once mounted,
  // it should be possible to get the pictures.
  if(IsSCSIDisk(data,usbdev))
  {
  	// if there is no volume name, use a default
  	if (!strlen(msdDeviceName))
    	return false;
  	else 
    	// create mount point at root
    	sprintf(tmp, "/%s",msdDeviceName);
  	// make sure name is unique or if not unique,
  	// make sure existing directory is empty 
  	strcpy(mount_point, tmp);
  	while (1) 
  	{
    	if (mkdir(mount_point, 0777) >= 0)
      		break;
    	directory.SetTo(mount_point);
    	if ((!directory.InitCheck()) &&(!directory.CountEntries()))
      		break;
    sprintf(mount_point, "%s%d", tmp, index++);
  	}
  	// try mounting 
  	if (mount("dos", mount_point,device_path, 0, NULL, 0) < 0) 
   	{	
   		MSD_logError(MS_OPEN_CAMERA);
   		return false;
   	}
   	strcpy(msdMountPoint,mount_point);
  	camConnected = true;
  	return true;
  }
  else
  	return false;
}
//
// MSDInterface :: Unmount the correct SCSI device
bool MSDInterface::SCSIUnmount()
{
	// try unmounting 
  	if(unmount(msdDeviceName) < 0)
   	{
   		MSD_logError(MS_CLOSE_CAMERA);
   		return false;
   	}
   	delete(msdMountPoint);
  	return true;
}
//
// MSDInterface :: Recursively wander down from a path, looking for "raw"
// devices to call inquiry on and return the the mount name
bool MSDInterface::SCSIMount(const char *path,BUSBDevice *device)
{
  BDirectory dir(path);
  if(dir.InitCheck() == B_OK)
  {
    BEntry entry;
    while(dir.GetNextEntry(&entry,true) >= 0) 
    {
      BPath name;
      entry.GetPath(&name);
      if(entry.IsDirectory()) 
        SCSIMount(name.Path(),device);
      else if(!strcmp(name.Leaf(),"raw"))
      {
        if(SCSIProbeMount(name.Path(),device))
        {
        	getMSDItems(msdMountPoint);
        	return true;
        }
      }
    }
    return false;
  }
  else
  	return false;
}
//
//		MSDInterface: IsSCSIDisk
bool	MSDInterface::IsSCSIDisk(uchar data[36],BUSBDevice *usbdev)
{
	int type;
	BString vendor,device, usb_vendor;
	// get the type of the scsi device
	type = data[0] & 0x1F;
	// get the vendor of the scsi device
  	vendor.Insert((char*)data+8,8,0);
  	//get the device name of the scsi device
  	device.Insert((char*)data+16,16,0);
  	//get the usb vendorname
  	usb_vendor.Insert(usbdev->ManufacturerString(),8,0);
  	// check if the scsi device is the mass storage device
  	// we are looking for
  	if(type == 0)
  		if((device.FindFirst("Camera") > B_ERROR) || (vendor.FindFirst("FUJIFILM") > B_ERROR))
  		{	
  			// get the device name
  			strncpy(msdDeviceName,(char*)data+8,8);
  			int i = 7;
  			while(i >= 0)
 			{
  				if(msdDeviceName[i] == ' ')
  					msdDeviceName[i] = '\0';
  				else
  					break;
  				i--;
  			}
  			// get the version
  			strncpy(msdVersion,(char*)data+32,4);
  			i = 3;
  			while(i >= 0)
 			{
  				if(msdVersion[i] == ' ')
  					msdVersion[i] = '\0';
  				else
  					break;
  				i--;
  			}
  			return true;
  		}
	return false;
}
//
//	MSDInterface:RescaleBitmap
void MSDInterface::RescaleBitmap(char *srcfile, BBitmap *dest)
{
	#ifdef DEBUG
	FILE	*file;
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Begin rescaling \n");
	fclose(file);
	#endif
	//BRect rect(0,150,0,113);
	BRect rect(0,0,150,113);
	dest = new BBitmap(rect,B_RGB32,true);
	BitmapView *bmview = new BitmapView(rect);
	//BView *bmview = new BView(rect, "BitmapView", B_FOLLOW_ALL_SIDES,0);
	bmview->LoadBitmap(srcfile);
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Bitmap loaded\n");
	fclose(file);
	#endif
	dest->AddChild(bmview);
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Child added\n");
	fclose(file);
	#endif
	dest->Lock();
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Lock dest\n");
	fclose(file);
	#endif
	bmview->Invalidate();
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Invalidate view\n");
	fclose(file);
	#endif
	bmview->Sync();
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Sync view\n");
	fclose(file);
	#endif
	bmview->RemoveSelf();
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: Remove view\n");
	fclose(file);
	#endif
	dest->Unlock();
	delete(bmview);
	#ifdef DEBUG
	file = fopen(LOGFILE,"a");
	fprintf(file,"MS - RescaleBitmap :: End rescaling \n");
	fclose(file);
	#endif
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
