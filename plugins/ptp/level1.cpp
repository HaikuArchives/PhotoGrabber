/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//		System Includes
#include <string.h>
#include <iostream>
#include <NodeInfo.h>
#include <libexif/exif-data.h>
#include <libexif/exif-entry.h>
//
//		Local Includes
#include "level1.h"
#include "logger.h"

//		USB Roster class
class Roster : public BUSBRoster
{
public:
virtual status_t DeviceAdded(BUSBDevice *dev)
{
	// Check if it is a class 6 device
	uint32 i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	LogDebug("PTP - Added device location is %s.",dev->Location());
	for(i=0;i<(int)dev->CountConfigurations();i++)
	{
		conf = dev->ConfigurationAt(i);
		if(conf)
		{
			for(j=0;j<(int)conf->CountInterfaces();j++)
			{
				ifc = conf->InterfaceAt(j);	
				if(ifc)
					if(ifc->Class() == 6)
					{
						if(dev->InitCheck() || dev->SetConfiguration(dev->ConfigurationAt(i)))
							LogError("PTP - No PTP device is present.n");
						else 
						{
							LogDebug("PTP - Device %s found and attached!",dev->ProductString());
							// Allocate memory for PTP parameters
							params = (PTPParams*)malloc(sizeof(PTPParams));
							memset(params,0,sizeof(PTPParams));
							//
							cameraDevice->configuration = i;
							cameraDevice->interface = j;
							uint32 k = ifc->CountEndpoints();
							while (k > 0) 
							{
								k--;
								if(ifc->EndpointAt(k)->IsBulk())
								{
									if(ifc->EndpointAt(k)->IsInput())
									{
										cameraDevice->bulkInput = k;
										params->maxpacketsize = ifc->EndpointAt(k)->MaxPacketSize();
									}
									else 
										cameraDevice->bulkOutput = k;
								}
								else if(ifc->EndpointAt(k)->IsInterrupt())
									cameraDevice->interruptInput = k;
								
							};
							cameraDevice->device = dev;
							cameraDevice->location = (char *)dev->Location();
							params->data = cameraDevice;	
							// Initialize the PTP parameters
							if(ptp_init_usb(params) == PG_OK)
							{ 
								LogDebug("PTP - Camera device location is %s.",cameraDevice->location);
								// send a message to the system core
								BMessage *core_msg;
								core_msg = new BMessage(CAM_CONNECTED);
								core_msg->AddString("product",dev->ProductString());
								if(msgtarget != NULL)
								{
									LogDebug("PTP - Send message to the system core.");
									msgtarget->PostMessage(core_msg);
								}
								return B_OK;
							}
						}
					}
			}
		}
	}
	if(cameraDevice != NULL)
	{
	LogDebug("PTP - Camera device location is %s.",cameraDevice->location);
	}
	return B_ERROR;
}
virtual void DeviceRemoved(BUSBDevice *dev)
{
	
	LogDebug("PTP - Camera device location is %s.",cameraDevice->location);
	LogDebug("PTP - Removed device location is %s.",dev->Location());
	if(cameraDevice->location == dev->Location())
	{
		if(ptp_exit_usb(params) == PG_OK)
		{
			free(params);
			// send a message to the system core
			BMessage *core_msg;
			core_msg = new BMessage(CAM_DISCONNECTED);
			core_msg->AddString("product",dev->ProductString());
			if(msgtarget != NULL)
			{
				LogDebug("PTP - Removed %s @ '%s'.",dev->IsHub() ? "hub" : "device", dev->Location());
				LogDebug("PTP - Send message to the system core.");
				msgtarget->PostMessage(core_msg);
			}
		}
	}
}
};

int get_BDCP_API_Revision(void)
{
	return(2);
}

void getPluginVersion(version_info &ver)
{
	ver.major = 2;
	ver.middle = 2;
	ver.minor = 2;
	ver.variety = 0;
	ver.internal = 0;
	sprintf(ver.short_info,"Jan-Rixt Van Hoye 2010");
	sprintf(ver.long_info,"PTP Cameras Plugin");
}

void getSupportedCameras(std::vector<std::string> & listofcams)
{
	listofcams.push_back("PTP (Picture Transfer Protocol) camera");
}

status_t openCamera(void)
{
	// Check USB Devices
	LogDebug("PTP - Start the roster.");
	// Allocate memory for the Camera Device
	cameraDevice = (USBCameraDevice *)malloc(sizeof(USBCameraDevice));
	memset(cameraDevice,0,sizeof(USBCameraDevice));
	//
	roster = new Roster;
	roster->Start();
	LogDebug("PTP - Roster started.");
	return(B_NO_ERROR);
}

status_t closeCamera(void)
{
	// Close the camera
	LogDebug("PTP - Close PTP plugin.");
	LogDebug("PTP - Stopping USB Roster.");
	roster->Stop();
	LogDebug("PTP - USB Roster stopped.");
	delete(roster);
	free(cameraDevice);
	LogDebug("PTP - USB Roster deleted.");
	return(B_NO_ERROR);
}

status_t getNumberofPics(int &number)
{
	//n = 0;
	number = 0;
	LogDebug("PTP - Get number of pictures.");
	getObjects(0xffffffff,0x000000);
	number = (*params).handles.n;
	LogDebug("PTP - There are %d items found to show!",number);
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum)
{
	LogDebug("PTP - Set current picture.");
	currentpicturenumber = picturenum;
	LogDebug("PTP - Current picnumber is: %d.",picturenum);
	LogDebug("PTP - Current (handler) is: %d.",currentpicturenumber);
	return B_NO_ERROR;
}
//
// Get the handles of a certain storage ID
status_t getObjects(uint32_t storage, uint32_t association)
{
	PTPObjectHandles *handles = (PTPObjectHandles *)malloc(sizeof(PTPObjectHandles));
	
	ptp_getobjecthandles(params, storage, 0x000000, association,handles);
	if(handles->n == 0)
	{
		LogDebug("PTP - No objecthandles were found.",handles->n);
		return B_ERROR;
	}
	else
	{
		LogDebug("PTP - There are %d objecthandles.",(*handles).n);
		for (uint32 j = 0; j < handles->n;j++)
		{
			addObject(handles->Handler[j]);
		}
	}
	free(handles);
	return B_NO_ERROR;
}

status_t downloadPicture(BPath savedir, const char *name)
{
	char *filename;
	long int size=0;
	
	LogDebug("PTP - Download picture with name: %s.", name);
	
	if(savedir != NULL)
	{
		int32 pathLength = strlen(savedir.Path());
		int32 fileNameLength = strlen((*params).objectinfo[currentpicturenumber].Filename);
		off_t file_size = 0;
		status_t err;
		BFile *fh;
		int numberOfCopies = 1;
		while(numberOfCopies <= 100)
		{
			filename = new char[pathLength + fileNameLength + 3];
			strcpy(filename,savedir.Path());
			strcat(filename,"/");
			if(name != NULL)
				strcat(filename,name);
			else
				strcat(filename,(*params).objectinfo[currentpicturenumber].Filename);
			if(numberOfCopies > 1)
				sprintf(filename,"%s %d",filename,numberOfCopies);
			// Check if the file exists. If it exists, check if it is empty or not.
			fh = new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE);
			err = fh->GetSize(&file_size);
			if(err == B_OK && file_size == 0)
			{
				delete fh;
				return saveCamPicture(filename);
			}
			else
				delete fh;
			numberOfCopies++;	
		}	
	}
	else
		LogDebug("PTP - Save directory is empty!");
	return(B_ERROR);
}
//
//		Save the picture
bool saveCamPicture (const char *filename)
{
	int					systemresult;
	BFile				*fh;
	BNodeInfo			*ni;
	int fd;
	int ret=0;
	
	LogDebug("PTP - Saving file %s.", filename);
	fd = open(filename, B_WRITE_ONLY | B_CREATE_FILE);
	LogDebug("PTP - File descriptor is: %d.", fd);
	if(fd > 0)
	{
		ret = ptp_getobject_tofd(params,(*params).handles.Handler[currentpicturenumber],fd);
		close(fd);
		if( ret == PTP_RC_OK)
		{
			if((fh=new BFile(filename, B_WRITE_ONLY)))
			{
				long int			fherr;
		
				if((fherr = fh->InitCheck()) != B_OK)
					return(B_ERROR);
				if((ni=new BNodeInfo(fh)))
				{
					switch((*params).objectinfo[currentpicturenumber].ObjectFormat)
					{
						case PTP_OFC_AIFF:
						{
							ni->SetType("audio/aiff");
							break;
						}
						case PTP_OFC_WAV:
						{
							ni->SetType("audio/wav");
							break;
						}
						case PTP_OFC_MP3:
						{
							ni->SetType("audio/mpeg3");
							break;
						}
						case PTP_OFC_AVI:
						{
							ni->SetType("video/avi");
							break;
						}
						case PTP_OFC_MPEG:
						{
							ni->SetType("video/mpeg");
							break;
						}
						case PTP_OFC_ASF:
						{
							ni->SetType("video/x-ms-asf");
							break;
						}
						case PTP_OFC_QT:
						{
							ni->SetType("video/quicktime");
							break;
						}
						case PTP_OFC_EXIF_JPEG:
						case PTP_OFC_CIFF:
						case PTP_OFC_JFIF:
						{
							ni->SetType("image/jpeg");
							break;
						}
						case PTP_OFC_TIFF:
						case PTP_OFC_TIFF_EP:
						case PTP_OFC_TIFF_IT:
						{
							ni->SetType("image/tiff");
							break;
						}
						case PTP_OFC_FlashPix:
						{
							ni->SetType("image/vnd.fpx");
							break;
						}
						case PTP_OFC_BMP:
						{
							ni->SetType("image/bmp");
							break;
						}
						case PTP_OFC_GIF:
						{
							ni->SetType("image/gif");
							break;
						}
						case PTP_OFC_PCD:
						{
							ni->SetType("image/pcd");
							break;
						}
						case PTP_OFC_PICT:
						{
							ni->SetType("image/pict");
							break;
						}
						case PTP_OFC_PNG:
						{
							ni->SetType("image/png");
							break;
						}
						case PTP_OFC_JP2:
						{
							ni->SetType("image/jp2");
							break;
						}
						case PTP_OFC_JPX:
						{
							ni->SetType("image/jpx");
							break;
						}
						default:
						{
							ni->SetType("image/jpeg");
						}
					}
									
					delete ni;
				}
				// Create the EXIF data as attributes
				FILE *exifFile = fopen(filename, "rb");
				unsigned char buf[0x7fff];
				ssize_t bufsize = fread(buf, 1, sizeof(buf), exifFile);
				if (bufsize == 0) 
				{
					fprintf (stderr, "Error reading file.\n");
					fclose(exifFile);
					delete fh;
					return (B_ERROR);
				}
				ExifData *data = exif_data_new_from_data(buf, bufsize);
				if (!data)
				{ 
					fclose(exifFile);
					delete fh;
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
							fh->WriteAttr(ident.String(), B_STRING_TYPE, 0, (const void *)&value, strlen(value) + 1);
							count++;
						}
					}
				}
				exif_data_unref(data);
				fclose(exifFile);
				delete fh;
			}
		}
		else
			return B_ERROR;
	}
	else
		return B_ERROR;
	systemresult=-1;
	return (B_NO_ERROR);
}
//
// Add a object to the handles array
status_t addObject(uint32_t handle)
{
	int n;
	PTPObjectInfo *objectinfo =(PTPObjectInfo *)malloc(sizeof(PTPObjectInfo));
	memset(objectinfo,0, sizeof(PTPObjectInfo));
	
	LogDebug("PTP - Adding object.");
	
	ptp_getobjectinfo(params,handle, objectinfo);
	if((*objectinfo).ObjectFormat != PTP_OFC_Undefined 
		&& (*objectinfo).ObjectFormat != PTP_OFC_Association 
		&& (*objectinfo).ObjectFormat != PTP_OFC_DPOF)
	{
		n= ++params->handles.n;
		params->objectinfo = (PTPObjectInfo*) realloc(params->objectinfo, sizeof(PTPObjectInfo)*n);
		params->handles.Handler=(uint32_t *)realloc(params->handles.Handler,sizeof(uint32_t)*n);
		memset(&params->objectinfo[n-1],0,sizeof(PTPObjectInfo));
		params->handles.Handler[n-1]=handle;
		ptp_getobjectinfo(params,handle, &params->objectinfo[n-1]);
	}
	
	free(objectinfo);
	return B_NO_ERROR;
}
