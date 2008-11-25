/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
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
FILE *lflevel2;

status_t setMessageTarget(BLooper* mtarget)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");
		if(mtarget != NULL)	
			fprintf(lflevel2,"PTP - Message Target is set\n");
		else
			fprintf(lflevel2,"PTP - Message Target is NOT set\n");
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
		fprintf(lflevel2,"PTP - Delete Picture\n");
		fclose(lflevel2);
	#endif
	ret = ptp_deleteobject(params,(*params).handles.Handler[currentpicturenumber],0);
	if(ret == PTP_RC_OK)
	{
        return(B_NO_ERROR);
    }
    logError(PTPCAM_DEL_PIC_FAIL);
	return(B_ERROR);	
}

status_t takePicture()
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Take picture\n");
		fclose(lflevel2);
	#endif
	return(B_NO_ERROR);
}

status_t getImageName(char* &name)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get image name from picture %d\n",currentpicturenumber);
		fclose(lflevel2);
	#endif
	name = (*params).objectinfo[currentpicturenumber].Filename;
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Image name: %s\n",name);
		fclose(lflevel2);
	#endif
	if(name == "")
	{
		logError(PTPCAM_GET_NAME_FAIL);
		return(B_ERROR);
	}
	return(B_NO_ERROR);
}

status_t getImageSize(int &size)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get image size\n");
		fclose(lflevel2);
	#endif
	size = (*params).objectinfo[currentpicturenumber].ObjectCompressedSize;
	return(B_NO_ERROR);
}

status_t getImageDate(char* &date)
{
	struct tm *ptr;
	char *tmpdate;
	tmpdate = new char[10];
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get image date\n");
		fclose(lflevel2);
	#endif
	ptr = gmtime(&(*params).objectinfo[currentpicturenumber].CaptureDate);
	strftime(tmpdate,100,"%d/%m/%Y",ptr);
	date = tmpdate;
	return(B_NO_ERROR);
}

status_t getThumbnail(BBitmap* & bitmap)
{
	int 		ret = 0;
	unsigned char	*image = NULL;
	long int 	size=0,width=0,height=0;
	
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get thumbnail\n");
		fclose(lflevel2);
	#endif
	ret = ptp_getthumb(params,(*params).handles.Handler[currentpicturenumber],&image);
	if(ret == PTP_RC_OK)
	{
		#ifdef DEBUG
			lflevel2 = fopen(LOGFILE,"a");	
			fprintf(lflevel2,"PTP - ptp_getthumb function went ok\n");
			fclose(lflevel2);
		#endif
		size = (*params).objectinfo[currentpicturenumber].ThumbCompressedSize;
		width = (*params).objectinfo[currentpicturenumber].ThumbPixWidth;
		height = (*params).objectinfo[currentpicturenumber].ThumbPixHeight;
		#ifdef DEBUG
			lflevel2 = fopen(LOGFILE,"a");
			fprintf(lflevel2,"PTP - conversion OK\n");
			fclose(lflevel2);
		#endif
		BMemoryIO in(image,size);
		bitmap = BTranslationUtils::GetBitmap(&in);
		#ifdef DEBUG
			lflevel2 = fopen(LOGFILE,"a");
			fprintf(lflevel2,"PTP - insert into bitmap OK\n");
			fclose(lflevel2);
		#endif
		delete(image);
		image = NULL;
		return(B_NO_ERROR);
	}
	else if(ret == PTP_RC_NoThumbnailPresent)
	{
		#ifdef DEBUG
			lflevel2 = fopen(LOGFILE,"a");
			fprintf(lflevel2,"No thumbnail present\n");
			fclose(lflevel2);
		#endif
	}
	logError(PTPCAM_GETPIC_FAIL);
	return(B_ERROR);
}	

status_t getImageHeight(int &height)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get image height\n");
		fclose(lflevel2);
	#endif
	height = (*params).objectinfo[currentpicturenumber].ImagePixHeight;
	return(B_NO_ERROR);
}

status_t getImageWidth(int &width)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - Get image width\n");
		fclose(lflevel2);
	#endif
	width = (*params).objectinfo[currentpicturenumber].ImagePixWidth;
	return(B_NO_ERROR);
}

status_t getDeviceType(int &type)
{
	#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");		
		fprintf(lflevel2,"PTP - Get the type of device\n");
		fclose(lflevel2);
	#endif
	type =  TYPE_USB;
	return(B_NO_ERROR);
}
/*BWindow* configurePlugin(BPoint middle)
{
	return(new configplugin(BRect(middle.x - 74,middle.y - 122,
								  middle.x + 74,middle.y + 122 )));
}*/
