/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// Local includes
#include "level2.h"
//#include "debug.h"
#include "logger.h"
//
// Includes
#include <cstdio>
#include <DataIO.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
 
extern BLooper* msgtarget;
extern MSInterface *msInterface;

status_t setMessageTarget(BLooper* mtarget)
{
	if(mtarget != NULL)	
		LogDebug("MASS - Message Target is set.");
	else
		LogDebug("MASS - Message Target is NOT set.");
	msgtarget = mtarget;
	return(B_NO_ERROR);
}

status_t deletePicture()
{
	int ret=0;
	LogDebug("MASS - Delete Picture.");
	if(!msInterface->deleteItem())
    {
    	LogError("MASS - Delete picture failed.");
		return(B_ERROR);	
    }
    return(B_NO_ERROR);
}

status_t takePicture()
{
	LogDebug("MASS - Take picture.");
	return(B_NO_ERROR);
}

status_t getImageName(char* &name)
{
	name = msInterface->getName();
	if(name == "")
	{
		LogError("MASS - Couldn't get the name of the item.");
		return(B_ERROR);
	}
	return(B_NO_ERROR);
}

status_t getImageSize(int &size)
{
	LogDebug("MASS - Get image size.");
	size = msInterface->getSize();
	return(B_NO_ERROR);
}

status_t getImageDate(char* &date)
{
	LogDebug("MASS - Get image date.");
	date = msInterface->getDate();
	return(B_NO_ERROR);
}

status_t getThumbnail(BBitmap* &bitmap)
{
	
	LogDebug("MASS - Get thumbnail.");
	bitmap = msInterface->getThumb();
	
	return(B_NO_ERROR);
}	

status_t getImageHeight(int &height)
{
	LogDebug("MASS - Get image height.");
	height = msInterface->getYRes();
	return(B_NO_ERROR);
}

status_t getImageWidth(int &width)
{
	LogDebug("MASS - Get image width.");
	width = msInterface->getXRes();
	return(B_NO_ERROR);
}

status_t getDeviceType(int &type)
{
	LogDebug("MASS - Get the type of device.");
	type =  TYPE_USB;
	return(B_NO_ERROR);
}
