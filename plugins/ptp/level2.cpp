/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// Local Includes
#include "level2.h"
#include "logger.h"
//
// Includes
#include <cstdio>
#include <DataIO.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
 
extern BLooper* msgtarget;

status_t setMessageTarget(BLooper* mtarget)
{
	if(mtarget != NULL)	
		LogDebug("PTP - Message Target is set.");
	else
		LogDebug("PTP - Message Target is NOT set.");
	msgtarget = mtarget;
	return(B_NO_ERROR);
}

status_t deletePicture()
{
	int ret=0;
	LogDebug("PTP - Delete Picture.");
	ret = ptp_deleteobject(params,(*params).handles.Handler[currentpicturenumber],0);
	if(ret == PTP_RC_OK)
	{
        return(B_NO_ERROR);
    }
    LogError("PTP - Delete picture failed.");
	return(B_ERROR);	
}

status_t takePicture()
{
	LogDebug("PTP - Take picture.");
	return(B_NO_ERROR);
}

status_t getImageName(char* &name)
{
	LogDebug("PTP - Get image name from picture %d.",currentpicturenumber);
	name = (*params).objectinfo[currentpicturenumber].Filename;
	LogDebug("PTP - Image name: %s.",name);
	if(name == "")
	{
		LogError("PTP - Could not get picture name.");
		return(B_ERROR);
	}
	return(B_NO_ERROR);
}

status_t getImageSize(int &size)
{
	LogDebug("PTP - Get image size.");
	size = (*params).objectinfo[currentpicturenumber].ObjectCompressedSize;
	return(B_NO_ERROR);
}

status_t getImageDate(char* &date)
{
	struct tm *ptr;
	char *tmpdate;
	tmpdate = new char[10];
	LogDebug("PTP - Get image date.");
	ptr = gmtime(&(*params).objectinfo[currentpicturenumber].CaptureDate);
	strftime(tmpdate,100,"%d/%m/%Y",ptr);
	date = tmpdate;
	return(B_NO_ERROR);
}

status_t getThumbnail(BBitmap* & bitmap)
{
	int 			ret = 0;
	unsigned char	*image = NULL;
	long int 		size=0,width=0,height=0;
	
	LogDebug("PTP - Get thumbnail.");

	ret = ptp_getthumb(params,(*params).handles.Handler[currentpicturenumber],&image);
	if(ret == PTP_RC_OK)
	{
		LogDebug("PTP - ptp_getthumb function went ok.");

		size = (*params).objectinfo[currentpicturenumber].ThumbCompressedSize;
		width = (*params).objectinfo[currentpicturenumber].ThumbPixWidth;
		height = (*params).objectinfo[currentpicturenumber].ThumbPixHeight;
		LogDebug("PTP - conversion OK.");

		BMemoryIO in(image,size);
		bitmap = BTranslationUtils::GetBitmap(&in);
		LogDebug("PTP - insert into bitmap OK.");
		delete(image);
		image = NULL;
		return(B_NO_ERROR);
	}
	else
	{
		LogDebug("PTP - No thumbnail present.");
		bitmap = NULL;
	}
	LogError("PTP - Could not get the thumbnail from the device.");
	return(B_ERROR);
}	

status_t getImageHeight(int &height)
{
	LogDebug("PTP - Get image height.");

	height = (*params).objectinfo[currentpicturenumber].ImagePixHeight;
	return(B_NO_ERROR);
}

status_t getImageWidth(int &width)
{
	LogDebug("PTP - Get image width.");

	width = (*params).objectinfo[currentpicturenumber].ImagePixWidth;
	return(B_NO_ERROR);
}

status_t getDeviceType(int &type)
{
	LogDebug("PTP - Get the type of device.");

	type =  TYPE_USB;
	return(B_NO_ERROR);
}
