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
#include "logger.h"
//
//		External variables
FILE *lfcam;
//
// Camera::constructor
Camera::Camera(char *libName) 
  : BLooper("cameralooper") 
{
    // First create the temporary directory
    BDirectory tmpDir("/boot");
    tmpDir.CreateDirectory("var/tmp",&tmpDir);
    LogDebug("CAM - Plugin name is: %s.",libName);
    // Create the interface
    camInterface = new CamInterface(libName);
}
//
// Camera::destructor
Camera::~Camera() {
    delete(camInterface);
}
//
//	Camera:: Start the camera looper
bool Camera::Start() {
    // Give the system core looper to the plugin. 
    // Then the plugin is able to send messages to the core system
    LogDebug("CAM - Start listening to connecting digital cameras.");
    BLooper::Run();
    return B_OK;
}
//
//	Camera:: Stop the camera looper
bool 
Camera::Stop() {
    LogDebug("CAM - Stop listening to connecting digital cameras.");
    BLooper::Quit();
    return B_OK;
}
//
//	Camera:: Open Device
bool 
Camera::OpenDevice() {
    LogDebug("CAM - Open the digital camera.");
    camInterface->setCoreSystemLoop(this);
    return camInterface->open();
}
//
//	Camera:: Close Device
bool 
Camera::CloseDevice() {
    LogDebug("CAM - Close the digital camera.");
    return	camInterface->close();
}
//
// Camera:: get number of pictures
int	
Camera::GetNumberOfItems() {	
    return camInterface->getNumberOfItems();		
}
// 
//	Camera::Get the item bitmap
bool 
Camera::DownloadItem(uint32 itemhandle, entry_ref *copyToDir = NULL, const char *fileName = NULL) {
    BPath directory;
    // Save directory bepalen
    if(copyToDir != NULL)
	directory = BPath(copyToDir);
    else
	directory = CameraSavedir;
    LogDebug("CAM - Download item with name %s.", fileName);
    return	camInterface->downloadItem(itemhandle,directory, fileName);
}
//
// 	Camera:: delete an item
bool 
Camera::RemoveItem(uint32 itemhandle) {	
    return camInterface->deleteItem(itemhandle);
}
// 
//	Camera::get the data of all the items of the camera
bool 
Camera::GetCameraItems() {	
    ItemData *localItemData;
    BMessage *message;

    uint32 numberOfItems = camInterface->getNumberOfItems();
    if(numberOfItems == 0) {
	LogDebug("CAM - There are no items on the camera.");
	message = new BMessage(GET_ITEMS_DONE);
	be_app->PostMessage(message, be_app);
	return(B_ERROR);
    } else {
        message = new BMessage(GET_ITEMS_START);
        message->AddFloat("total", numberOfItems);
	be_app->PostMessage(message, be_app);
	for (uint32 i = 0; i < numberOfItems;i++) {
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
	    if(localItemData->ItemThumbBitmap != NULL) {
			localItemData->ItemThumbXres = (uint32)localItemData->ItemThumbBitmap->Bounds().Width();
			localItemData->ItemThumbYres = (uint32)localItemData->ItemThumbBitmap->Bounds().Height();
	    } else {
			localItemData->ItemThumbXres = THUMBWIDTH;
			localItemData->ItemThumbYres = THUMBHEIGHT;
	    }
	    // send a message to insert a item in the list
	    message = new BMessage(ADD_ITEM);
	    message->AddPointer("item", localItemData);
	    be_app->PostMessage(message, be_app);
	    //
	    localItemData = NULL;
	}
	message = new BMessage(GET_ITEMS_DONE);
	be_app->PostMessage(message);
	return(B_OK);
    }	
}
//
// 	Camera:: Set the Download Properties
bool Camera::SetDownloadProps(BPath savedir) {
    CameraSavedir = savedir;
    BDirectory currentDir("/");
    currentDir.CreateDirectory(savedir.Path(),NULL);
    return true;
}
//
// 	Camera:: Get the type of the device
int Camera::GetDeviceType() {
    return camInterface->getDevType();
}
//
//	Camera::Get Device Properties
void Camera::GetDeviceProperties() {
    return;
}

bool 
Camera::GetDeviceCapability(pluginCaps caps) {
    return camInterface->hasFunction(caps);
}
//
//  Camera:: Receive message
void 
Camera::MessageReceived(BMessage *message) {	
    switch(message->what) {
	case DOWN_ITEM: {
	    int32 handle;
	    entry_ref copyToDir;
	    entry_ref *directory = NULL;
	    const char *fileName = NULL;
	    message->FindInt32("itemhandle",&handle);
	    if(message->FindRef("copyToDir",&copyToDir) >= 0)
		    directory = &copyToDir;
	    fileName = message->FindString("name");
	    LogDebug("CAM - Message received and string 'name' value is %s.", fileName);
	    if(DownloadItem((uint32)handle,directory,fileName) == B_OK) {
		BMessage reply(DOWN_ITEM_OK);
		message->SendReply(&reply);
	    }
	    break;
	}
	case REM_ITEM: {
	    int32 handle;
	    message->FindInt32("itemhandle",&handle);
	    if (RemoveItem((uint32)handle) == B_OK) {
		BMessage reply(REM_ITEM_OK);
		message->SendReply(&reply);
	    }
	    break;
	}
	case GET_ITEMS:
	    GetNumberOfItems();
	    GetCameraItems();
	    break;
	    
	case GET_ITEM_COUNT: {
	    LogDebug("CAM - Get item count.");
	    int numberOfItems = GetNumberOfItems();
	    BMessage reply(GET_ITEM_COUNT);
	    reply.AddInt32("numofitems", (int32)numberOfItems);
	    message->SendReply(&reply);
	    break;
	}
	case GET_DEVICE_TYPE: {
            int type = 0;
            if (camInterface)
                type = camInterface->getDevType();
                
	    LogDebug("CAM - Device type is: %d.",type);
	    BMessage reply(GET_DEVICE_TYPE);
	    reply.AddInt32("type", (int32)type);
	    message->SendReply(&reply);
	    break;
	}
	case OPEN_DEVICE: {
	    bool open = OpenDevice();
	    BMessage reply(OPEN_DEVICE);
	    reply.AddBool("open", open);
	    message->SendReply(&reply);
	    break;
	}
	case CLOSE_DEVICE: {
	    bool closed = CloseDevice();
	    BMessage reply(CLOSE_DEVICE);
	    reply.AddBool("closed", closed);
	    message->SendReply(&reply);
	    break;
	}
	case RELOAD_CONFIGURATION: {
	    const char *libName = NULL;
	    delete camInterface;
	    libName = message->FindString("libname");
	    camInterface = new CamInterface((char *)libName);
	    break;
	}
	case CHECK_FUNCTION : {
	    uint32 function = message->GetUInt32("function", 0xffff);
	    BMessage reply(CHECK_FUNCTION);
	    reply.AddBool("present", camInterface->hasFunction((pluginCaps)function));
	    message->SendReply(&reply);
	    break;
	}
    case CAM_TAKE_SHOT : {
        if (camInterface->takeItem()) {
            BMessage reply(CAM_SHOT_TAKEN);
            message->SendReply(&reply);
        }
        break;
    }
	case CAM_CONNECTED : {
	    int type = GetDeviceType();
	    message->AddInt32("type", (int32)type);	    
	    be_app->PostMessage(message);
	    break;
	}
	case CAM_DISCONNECTED :
	case GET_ITEMS_DONE:
	    be_app->PostMessage(message);
	    break;
	default: 
	    BLooper::MessageReceived(message);
	    break;
    }
}

