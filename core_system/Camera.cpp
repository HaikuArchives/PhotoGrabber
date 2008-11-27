/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//	Includes
#include <stdio.h>
//	Local includes
#include "Camera.h"
#include "core_system.h"
//
//		External variables
extern class BeDiGiCamApp *app;
FILE *lfcam;
//
// Camera::constructor
Camera::Camera(char *libName = NULL) : BLooper("cameralooper")
{
	// First create the temporary directory
	BDirectory tmpDir("/boot");
	tmpDir.CreateDirectory("var/tmp",&tmpDir);
	#ifdef DEBUG
		lfcam = fopen(LOGFILE,"a");
		fprintf(lfcam,"CAM - Plugin name is: %s.\n",libName);
		fclose(lfcam);
	#endif
	// Create the interface
	camInterface = new CamInterface(libName);
}
//
// Camera::destructor
Camera::~Camera()
{
	delete(camInterface);
}
//
//	Camera:: Start the camera looper
bool Camera::Start()
{
	// Give the system core looper to the plugin. 
	// Then the plugin is able to send messages to the core system
	#ifdef DEBUG
		lfcam = fopen(LOGFILE,"a");
		fprintf(lfcam,"CAM - Start listening to connecting digital cameras.\n");
		fclose(lfcam);
	#endif
	BLooper::Run();
	return B_OK;
}
//
//	Camera:: Stop the camera looper
bool Camera::Stop()
{
	#ifdef DEBUG
		lfcam = fopen(LOGFILE,"a");
		fprintf(lfcam,"CAM - Stop listening to connecting digital cameras.\n");
		fclose(lfcam);
	#endif
	BLooper::Quit();
	return B_OK;
}
//
//	Camera:: Open Device
bool Camera::OpenDevice()
{
	#ifdef DEBUG
		lfcam = fopen(LOGFILE,"a");
		fprintf(lfcam,"CAM - Open the digital camera.\n");
		fclose(lfcam);
	#endif
	camInterface->setCoreSystemLoop(app);
	return camInterface->open();
}
//
//	Camera:: Close Device
bool Camera::CloseDevice()
{
	#ifdef DEBUG
		lfcam = fopen(LOGFILE,"a");
		fprintf(lfcam,"CAM - Close the digital camera.\n");
		fclose(lfcam);
	#endif
	return	camInterface->close();
}
//
// Camera:: get number of pictures
int	Camera::GetNumberOfItems()
{	
	return camInterface->getNumberOfItems();		
}
// 
//	Camera::Get the item bitmap
bool Camera::DownloadItem(uint32 itemhandle, entry_ref *copyToDir = NULL, const char *fileName = NULL)
{
	BPath directory;
	// Save directory bepalen
	if(copyToDir != NULL)
	 	directory = BPath(copyToDir);
	else
		directory = CameraSavedir;
	return	camInterface->downloadItem(itemhandle,directory, fileName);
}
//
// 	Camera:: delete an item
bool Camera::RemoveItem(uint32 itemhandle)
{	
	return	camInterface->deleteItem(itemhandle);
}
// 
//	Camera::get the data of all the items of the camera
bool Camera::GetCameraItems()
{	
	ItemData *localItemData;
	BMessage *message;

	uint32 numberOfItems = camInterface->getNumberOfItems();
	if(numberOfItems == 0)
	{
		#ifdef DEBUG
			lfcam = fopen(LOGFILE,"a");
			fprintf(lfcam,"CAM - There are no items on the camera.\n");
			fclose(lfcam);
		#endif
		message = new BMessage(GET_ITEMS_DONE);
		app->PostMessage(message,app);
		return(B_ERROR);
	}
	else
	{
		for (uint32 i = 0; i < numberOfItems;i++)
		{
			
			localItemData = new ItemData();
			// set current item is important! Don't forget it!
			camInterface->setCurrentItem(i);
			localItemData->ItemHandle = i;
			localItemData->ItemSize = camInterface->getSize();
			localItemData->ItemName = camInterface->getName();
			localItemData->ItemXres = camInterface->getWidth();
			localItemData->ItemYres = camInterface->getHeight();
			localItemData->ItemDate = camInterface->getDate();
			localItemData->ItemThumbBitmap = camInterface->getThumb();
			if(localItemData->ItemThumbBitmap != NULL)
			{
				localItemData->ItemThumbXres = (uint32)localItemData->ItemThumbBitmap->Bounds().Width();
				localItemData->ItemThumbYres = (uint32)localItemData->ItemThumbBitmap->Bounds().Height();
			}
			else
			{
				localItemData->ItemThumbXres = THUMBWIDTH;
				localItemData->ItemThumbYres = THUMBHEIGHT;
			}
			// send a message to insert a item in the list
			message = new BMessage(ADD_ITEM);
			message->AddPointer("item",localItemData);
			app->PostMessage(message,app);
			//
			localItemData = NULL;
		}
		message = new BMessage(GET_ITEMS_DONE);
		app->PostMessage(message,app);
		return(B_OK);
	}	
}
//
// 	Camera:: Set the Download Properties
bool Camera::SetDownloadProps(BPath savedir)
{
	CameraSavedir = savedir;
	BDirectory currentDir("/");
	currentDir.CreateDirectory(savedir.Path(),NULL);
	return true;
}
//
// 	Camera:: Get the type of the device
int Camera::GetDeviceType()
{
	return camInterface->getDevType();
}
//
//	Camera::Get Device Properties
void Camera::GetDeviceProperties()
{
	return;
}
//
//  Camera:: Receive message
void Camera::MessageReceived(BMessage *message)
{	
	switch(message->what)
	{
		case DOWN_ITEM:
		{
			int32 handle;
			entry_ref copyToDir;
			entry_ref *directory = NULL;
			const char *fileName = NULL;
			message->FindInt32("itemhandle",&handle);
			if(message->FindRef("copyToDir",&copyToDir) >= 0)
				directory = &copyToDir;
			fileName = message->FindString("name");
			if(DownloadItem((uint32)handle,directory,fileName) == B_OK)
			{
				BMessage reply(DOWN_ITEM_OK);
				message->SendReply(&reply);
			}
			break;
		}
		case REM_ITEM:
		{
			int32 handle;
			message->FindInt32("itemhandle",&handle);
			if(RemoveItem((uint32)handle) == B_OK)
			{
				BMessage reply(REM_ITEM_OK);
				message->SendReply(&reply);
			}
			break;
		}
		case GET_ITEMS:
			GetCameraItems();
			break;
		case GET_ITEM_COUNT:
		{
			#ifdef DEBUG
				lfcam = fopen(LOGFILE,"a");
				fprintf(lfcam,"CAM - Get item count.\n");
				fclose(lfcam);
			#endif
			int numberOfItems = GetNumberOfItems();
			BMessage reply(GET_ITEM_COUNT);
			reply.AddInt32("numofitems", (int32)numberOfItems);
			message->SendReply(&reply);
			break;
		}
		case GET_DEVICE_TYPE:
		{
			int type = GetDeviceType();
			#ifdef DEBUG
				lfcam = fopen(LOGFILE,"a");
				fprintf(lfcam,"CAM - Device type is: %d.\n",type);
				fclose(lfcam);
			#endif
			BMessage reply(GET_DEVICE_TYPE);
			reply.AddInt32("type", (int32)type);
			message->SendReply(&reply);
			break;
		}
		case OPEN_DEVICE:
		{
			bool open = OpenDevice();
			BMessage reply(OPEN_DEVICE);
			reply.AddBool("open", open);
			message->SendReply(&reply);
			break;
		}
		case CLOSE_DEVICE:
		{
			bool closed = CloseDevice();
			BMessage reply(CLOSE_DEVICE);
			reply.AddBool("closed", closed);
			message->SendReply(&reply);
			break;
		}
		case RELOAD_CONFIGURATION:
		{
			const char *libName = NULL;
			delete(camInterface);
			libName = message->FindString("libname");
			camInterface = new CamInterface((char *)libName);
			break;
		}
		default: 
			BLooper::MessageReceived(message);
			break;
	}
}

