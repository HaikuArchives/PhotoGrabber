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
//
// Camera::constructor
Camera::Camera(char *libName, char *looperName) : BLooper(looperName)
{
	CameraModel = "Not Available";
	CameraManufacturer = "Not Available";
	CameraVersion = "No Version";
	CameraSerialNumber = "No Serial Number";
	// First create the temporary directory
	BDirectory tmpDir("/boot");
	tmpDir.CreateDirectory("var/tmp",&tmpDir);
	// Create the interface
	camInterface = new CamInterface(libName);
	looperRunning = false;
}
//
// Camera::destructor
Camera::~Camera()
{
//	delete camInterface;
	camInterface = NULL;
}
//
//	Camera:: Open Camera
bool Camera::OpenCamera()
{
	// Give the system core looper to the plugin. 
	// Then the plugin is able to send messages to the core system
	camInterface->setCoreSystemLoop(app); 
	if(camInterface->open())
	{
		Run();
		looperRunning = true;
	}
	return camInterface->cameraConnected(); 
	
}
//
//	Camera:: Close Camera
bool Camera::CloseCamera()
{
	if(camInterface->cameraConnected())
		camInterface->close();
	looperRunning = false;
	Lock();
	Quit();
	return looperRunning;
}
//
//	Camera:: Has the camera been opened?
bool Camera::IsOpen()
{
	return looperRunning;
}
//
//	Camera:: Quit requested
bool Camera::QuitRequested()
{
	return BLooper::QuitRequested();
}
//
//	Camera:: Check Connection
bool Camera::CheckCameraConnection()
{
	return(camInterface->cameraConnected());
}
//
//	Camera::Get Model
const char* Camera::GetModel()
{
	return CameraModel.String();
}
//
//	Camera::Get Manufacturer
const char* Camera::GetManufacturer()
{
	return CameraManufacturer.String();
}//
//	Camera::Get Version
const char* Camera::GetVersion()
{
	return CameraVersion.String();
}//
//	Camera::Get Serial Number
const char* Camera::GetSerialNumber()
{
	return CameraSerialNumber.String();
}
//
//	Camera::set Model
void Camera::SetModel(char *model)
{
	if(model != "")
		CameraModel=model;
}
//
//	Camera::set Manufacturer
void Camera::SetManufacturer(char *manufac)
{
	if(manufac != "")
		CameraManufacturer=manufac;
}
//
//	Camera::set Version
void Camera::SetVersion(char *version)
{
	if(version != "")
		CameraVersion=version;
}
//
//	Camera::set Serial Number
void Camera::SetSerialNumber(char *serialnumber)
{
	if(serialnumber != "")
		CameraSerialNumber=serialnumber;
}
//
// Camera:: get number of pictures
int	Camera::GetNumberOfItems()
{	
	return camInterface->getNumberOfItems();		
}
// 
//	Camera:: open the camera info
bool Camera::GetCameraInfo()
{	
	version_info deviceVersion;
	char *versionString;
	// print the information of the camera on the screen
	SetModel("Camera");
	deviceVersion = camInterface->getVersion();
	SetManufacturer(deviceVersion.long_info);
	versionString = new char[100];
	sprintf(versionString,"%ld.%ld.%ld",deviceVersion.major,deviceVersion.middle,deviceVersion.minor);
	SetVersion(versionString);
	SetSerialNumber("000-000");
	return(true);
		
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
	camInterface->downloadItem(itemhandle,directory, fileName);
	return B_OK;
}
//
// 	Camera:: delete an item
bool Camera::RemoveItem(uint32 itemhandle)
{
	camInterface->deleteItem(itemhandle);
	return B_OK;
}
// 
//	Camera::get the data of all the items of the camera
bool Camera::GetCameraItems()
{	
	ItemData *localItemData;
	BMessage *message;
	if(CheckCameraConnection())
	{
		uint32 numberOfItems = camInterface->getNumberOfItems();
		if(numberOfItems == 0)
		{
			logCamError(CAM_NO_HANDLES);
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
	else
		return(B_ERROR);	
}
// 
//	Camera::Save the item
bool Camera::SaveCameraItem (char *data, long int size, const char filename[255])
{	
	return (B_NO_ERROR);
}
// 
//	Camera::open the camera storage info
bool Camera::GetStorageInfo(void)
{	
	return(B_OK);
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
		default: 
			BLooper::MessageReceived(message);
			break;
	}
}
//
//	Camera:: Logging
int Camera::logCamError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case CAM_NO_HANDLES:
			errorMessage = "BDCPCAM: There are no handles\n";
			break;
		default:
			errorMessage = "BDCPCAM: An unexpected error occured\n";
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

