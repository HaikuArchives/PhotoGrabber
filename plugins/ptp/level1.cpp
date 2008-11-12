/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
//		System Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <NodeInfo.h>

//		User Includes
#include "level1.h"
#include "level2.h"
#include "debug.h"
//
FILE *lflevel1;
//		USB Roster class
class Roster : public BUSBRoster
{
public:
virtual status_t DeviceAdded(BUSBDevice *dev)
{
	// Initialize the USB device
	int i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	
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
						#ifdef DEBUG
							lflevel1 = fopen(LOGFILE,"a");
							fprintf(lflevel1,"Device %s found and attached!\n",dev->ProductString());
							fclose(lflevel1);
						#endif
						appDev = dev;
						if(appDev->InitCheck() || appDev->SetConfiguration(appDev->ConfigurationAt(0)))
							logError(PTPCAM_DEV_NO_FIND);
						else
						{
							// initialize the ptp device
							if(PTP_init_ptp_usb(params,appDev) == B_OK)
							{ 
								ptp_opensession(params,1);
								// send a message to the system core
								BMessage *core_msg;
								core_msg = new BMessage(CAM_CONNECTED);
								core_msg->AddString("product",dev->ProductString());
								if(msgtarget != NULL)
								{
									#ifdef DEBUG
										lflevel1 = fopen(LOGFILE,"a");
										fprintf(lflevel1,"PTP: Send message to the system core\n");
										fclose(lflevel1);
									#endif
									msgtarget->PostMessage(core_msg);
								}
							}
						}
					}	
			}
		}
	}
	
	return B_OK;
}
virtual void DeviceRemoved(BUSBDevice *dev)
{
	// Initialize the USB device
	int i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	
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
						appDev = NULL;
						// send a message to the system core
						BMessage *core_msg;
						core_msg = new BMessage(CAM_DISCONNECTED);
						core_msg->AddString("product",dev->ProductString());
						if(msgtarget != NULL)
						{
							#ifdef DEBUG
								lflevel1 = fopen(LOGFILE,"a");
								fprintf(lflevel1,"PTP: Removed %s @ '%s'\n",dev->IsHub() ? "hub" : "device", dev->Location());
								fprintf(lflevel1,"PTP: Send message to the system core\n");
								fclose(lflevel1);
							#endif
							msgtarget->PostMessage(core_msg);
						}
					}
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
	ver.middle = 0;
	ver.minor = 2;
	ver.variety = 0;
	ver.internal = 0;
	sprintf(ver.short_info,"Jan-Rixt Van Hoye 2008");
	sprintf(ver.long_info,"BDCP PTP Cameras Plugin");
}

void getSupportedCameras(vector<string> & listofcams)
{
	listofcams.push_back("PTP(Picture Transfer Protocol) camera");
	listofcams.push_back("Nikon Coolpix 2000");
	listofcams.push_back("Nikon Coolpix SQ");
}

status_t openCamera(void)
{
	// Check USB Devices
	params = new PTPParams;
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Start the roster\n");
	fclose(lflevel1);
	#endif
	roster = new Roster;
	roster->Start();
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Roster started\n");
	fclose(lflevel1);
	#endif
	return(B_NO_ERROR);
}

status_t closeCamera(void)
{
	// Close the camera
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Close camera\n");
	fclose(lflevel1);
	#endif
	if(appDev != NULL)
		ptp_closesession(params);
	roster->Stop();
	return(B_NO_ERROR);
}

status_t getNumberofPics(int &number)
{
	int i=0;
	number = 0;
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Get number of pictures\n");
	fclose(lflevel1);
	#endif
	ptp_getobjecthandles(params, 0xffffffff, 0x000000, 0x000000,&params->handles);
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Got objecthandles\n");
	fclose(lflevel1);
	#endif
	if((*params).handles.n == 0)
	{
		logError(PTPCAM_NO_HANDLES);
		return(B_ERROR);
	}
	else
	{
		#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"PTP - Part1\n");
		fclose(lflevel1);
		#endif
		handles = new int[(*params).handles.n];
		(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo)* (*params).handles.n);
		#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"PTP - Part2\n");
		fclose(lflevel1);
		#endif
		for (uint32 j = 0; j < (*params).handles.n;j++)
		{
			ptp_getobjectinfo(params,(*params).handles.Handler[j],&params->objectinfo[j]);
			#ifdef DEBUG
			lflevel1 = fopen(LOGFILE,"a");
			fprintf(lflevel1,"PTP - Part2 - %d\n",j);
			fclose(lflevel1);
			#endif
			if((*params).objectinfo[j].ObjectFormat != PTP_OFC_Undefined && (*params).objectinfo[j].ObjectFormat != PTP_OFC_Association && (*params).objectinfo[j].ObjectFormat != PTP_OFC_DPOF)
			{
				handles[i] = j;
				i++;
			}
		}
		number = i;
		#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"PTP - Get part3\n");
		fclose(lflevel1);
		#endif
	}
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Number of pictures is: %d\n",number);
	fclose(lflevel1);
	#endif	
	return(B_NO_ERROR);
}

status_t setCurrentPicture(int picturenum)
{
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Set current picture\n");
	fclose(lflevel1);
	#endif
	currentpicturenumber = handles[picturenum];
	#ifdef DEBUG
	lflevel1 = fopen(LOGFILE,"a");
	fprintf(lflevel1,"PTP - Current picnumber is: %d\n",picturenum);
	fprintf(lflevel1,"PTP - Current picnumber(handle) is: %d\n",currentpicturenumber);
	fclose(lflevel1);
	#endif
	return(B_NO_ERROR);
}

status_t downloadPicture(BPath savedir, const char *name)
{
	char *image = NULL;
	char *filename;
	long int size=0;
	int ret=0;
	
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"PTP - Download picture\n");
		fclose(lflevel1);
	#endif
	if(savedir != NULL)
	{
		size=(*params).objectinfo[currentpicturenumber].ObjectCompressedSize;
		ret = ptp_getobject(params,(*params).handles.Handler[currentpicturenumber],&image);
		if ( ret == PTP_RC_OK)
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
					if(saveCamPicture(image,size,(*params).objectinfo[currentpicturenumber].ObjectFormat,filename) == B_NO_ERROR)
					{	
						image = NULL;
						return(B_NO_ERROR);
					}
				}
				else
					delete fh;
				numberOfCopies++;	
			}
		}	
	}
	else
	{
		#ifdef DEBUG
			lflevel1 = fopen(LOGFILE,"a");
			fprintf(lflevel1,"PTP - Save directory is empty!\n");
			fclose(lflevel1);
		#endif
	}
	return(B_ERROR);
}

bool saveCamPicture (char *data, long int size, uint16_t type,const char *filename)
{
	int					systemresult;
	BFile				*fh;
	BNodeInfo			*ni;
	
	#ifdef DEBUG
		lflevel1 = fopen(LOGFILE,"a");
		fprintf(lflevel1,"PTP - Save picture @ %s with size %d\n", filename,size);
		fclose(lflevel1);
	#endif
	if((fh=new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE )))
	{
		long int			fherr;

		if((fherr = fh->InitCheck()) != B_OK)
			return(B_ERROR);
		if((ni=new BNodeInfo(fh)))
		{
			// This has to be changed. It should set the correct type.
			#ifdef DEBUG
				lflevel1 = fopen(LOGFILE,"a");
				fprintf(lflevel1,"PTP - Object format%d\n",type);
				fclose(lflevel1);
			#endif
			switch(type)
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
		if(( (fherr = fh->Write(data, size)) != size))
		{
			delete fh;
			return(B_ERROR);			
		}
		else
		{
			delete fh;
		}
	}
	systemresult=-1;
	
	return (B_NO_ERROR);
}

