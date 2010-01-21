/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
#include "level2.h"
#include "debug.h"
#include "configplugin.h"
#include <cstdio>
#include <DataIO.h>
#include <BitmapStream.h>
#include <TranslatorRoster.h>
 
extern BLooper* msgtarget;
extern MSInterface *msInterface;
FILE *lflevel2;

status_t setMessageTarget(BLooper* mtarget)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");
		if(mtarget != NULL)	
			fprintf(lflevel2,"MS - Message Target is set\n");
		else
			fprintf(lflevel2,"MS - Message Target is NOT set\n");
		fclose(lflevel2);
	#endif
	msgtarget = mtarget;
	return(B_NO_ERROR);
}

status_t deletePicture()
{
	int ret=0;
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Delete Picture\n");
		fclose(lflevel2);
	#endif
	if(!msInterface->deleteItem())
    {
    	logError(MSCAM_DEL_PIC_FAIL);
		return(B_ERROR);	
    }
    return(B_NO_ERROR);
}

status_t takePicture()
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Take picture\n");
		fclose(lflevel2);
	#endif
	return(B_NO_ERROR);
}

status_t getImageName(char* &name)
{
	name = msInterface->getName();
	if(name == "")
	{
		logError(MSCAM_GET_NAME_FAIL);
		return(B_ERROR);
	}
	return(B_NO_ERROR);
}

status_t getImageSize(int &size)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get image size\n");
		fclose(lflevel2);
	#endif
	size = msInterface->getSize();
	return(B_NO_ERROR);
}

status_t getImageDate(char* &date)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get image date\n");
		fclose(lflevel2);
	#endif
	date = msInterface->getDate();
	return(B_NO_ERROR);
}

status_t getThumbnail(BBitmap* &bitmap)
{
	
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get thumbnail\n");
		fclose(lflevel2);
	#endif
	bitmap = msInterface->getThumb();
	
	return(B_NO_ERROR);
}	

status_t getImageHeight(int &height)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get image height\n");
		fclose(lflevel2);
	#endif
	height = msInterface->getYRes();
	return(B_NO_ERROR);
}

status_t getImageWidth(int &width)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get image width\n");
		fclose(lflevel2);
	#endif
	width = msInterface->getXRes();
	return(B_NO_ERROR);
}

status_t getDeviceType(int &type)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"MS - Get the type of device\n");
		fclose(lflevel2);
	#endif
	type =  TYPE_USB;
	return(B_NO_ERROR);
}
BWindow* configurePlugin(BPoint middle)
{
	return(new configplugin(BRect(middle.x - 74,middle.y - 122,
								  middle.x + 74,middle.y + 122 )));
}
