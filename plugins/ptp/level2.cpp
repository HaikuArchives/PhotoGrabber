/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
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
	//ptp_opensession(params,1);
	//ret = ptp_deleteobject(params,currentitemhandle,0);
	ret = ptp_deleteobject(params,(*params).handles.Handler[currentpicturenumber],0);
	//ptp_closesession(params);
	if(ret == PTP_RC_OK)
	{
    	/*// rearrange internal handles
    	#ifdef DEBUG
     	lflevel2 = fopen(LOGFILE,"a");	
	    fprintf(lflevel2,"PTP - Rearrange internal handles\n");
	    fclose(lflevel2);
	    #endif
        int i = 0;
    	while(handles[i] != NULL)
    	{
           if(handles[i] == currentitemhandle)
             handles[i] = -1;
           else if (handles[i] > currentitemhandle)
             handles[i] = handles[i] - 1;
           i++;
        }*/
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
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//name = (*params).objectinfo[0].Filename;
	name = (*params).objectinfo[currentpicturenumber].Filename;
	#ifdef DEBUG
	lflevel2 = fopen(LOGFILE,"a");	
	fprintf(lflevel2,"PTP - Image name: %s\n",name);
	fclose(lflevel2);
	#endif
	//ptp_closesession(params);
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
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//size = (*params).objectinfo[0].ObjectCompressedSize;
	size = (*params).objectinfo[currentpicturenumber].ObjectCompressedSize;
	//ptp_closesession(params);
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
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//ptr = gmtime(&(*params).objectinfo[0].CaptureDate);
	//ptp_closesession(params);
	ptr = gmtime(&(*params).objectinfo[currentpicturenumber].CaptureDate);
	strftime(tmpdate,100,"%d/%m/%Y",ptr);
	date = tmpdate;
	return(B_NO_ERROR);
}

status_t getThumbnail(BBitmap* & bitmap)
{
	int 		ret = 0;
	char 		*image = NULL;
	long int 	size=0,width=0,height=0;
	char		filename[40] = "/boot/var/tmp/thumb.tmp";
	BFile *file;
	
	#ifdef DEBUG
	lflevel2 = fopen(LOGFILE,"a");	
	fprintf(lflevel2,"PTP - Get thumbnail\n");
	fclose(lflevel2);
	#endif
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//ret = ptp_getthumb(params,currentitemhandle,&image);
	ret = ptp_getthumb(params,(*params).handles.Handler[currentpicturenumber],&image);
	if(ret == PTP_RC_OK)
	{
		#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");	
		fprintf(lflevel2,"PTP - ptp_getthumb function went ok\n");
		fclose(lflevel2);
		#endif
		//size = (*params).objectinfo[0].ThumbCompressedSize;
		//width = (*params).objectinfo[0].ThumbPixWidth;
		//height = (*params).objectinfo[0].ThumbPixHeight;
		size = (*params).objectinfo[currentpicturenumber].ThumbCompressedSize;
		width = (*params).objectinfo[currentpicturenumber].ThumbPixWidth;
		height = (*params).objectinfo[currentpicturenumber].ThumbPixHeight;
		#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");
		fprintf(lflevel2,"PTP - conversion OK\n");
		fclose(lflevel2);
		#endif
		//ptp_closesession(params);
		//
		if((file =  new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE)))
		{
			if(( (ret = file->Write(image, size)) != size))
			{
				delete file;
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - File write problem: %d\n", ret);
				fclose(lflevel2);
				#endif
				return(B_ERROR);			
			}
			else
			{
				delete(file);
				file = new BFile(filename,B_READ_ONLY);
			}
			BTranslatorRoster *proster = BTranslatorRoster::Default();
			if (!proster)
			{
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - TranslationRoster problem\n");
				fclose(lflevel2);
				#endif
				delete(file);
				return(B_ERROR);
			}
			translator_info info;
			memset(&info, 0, sizeof(translator_info));
			BMessage ioExtension;
			ret = proster->Identify(file, &ioExtension, &info, 0, NULL,B_TRANSLATOR_BITMAP);
			if(ret == B_NO_TRANSLATOR)
			{
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - No Translator found\n");
				fclose(lflevel2);
				#endif
				return(B_ERROR);
			}
			else if(ret == B_NOT_INITIALIZED)
			{
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - Not Initialized found\n");
				fclose(lflevel2);
				#endif
				return(B_ERROR);	
			}
			// Translate image data and create a new ShowImage window
			BBitmapStream outstream;
			if (proster->Translate(file, &info, &ioExtension, &outstream,B_TRANSLATOR_BITMAP) != B_OK)
			{
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - Translate problem\n");
				fclose(lflevel2);
				#endif
				delete(file);
				return(B_ERROR);
			}	
			if (outstream.DetachBitmap(&bitmap) != B_OK)
			{	
				#ifdef DEBUG
				lflevel2 = fopen(LOGFILE,"a");
				fprintf(lflevel2,"PTP - Detach problem\n");
				fclose(lflevel2);
				#endif
				delete(file);
				return(B_ERROR);
			}
		}
		else
		{
			logError(PTPCAM_NEW_FILE_FAIL);
			return(B_ERROR);
		}
		//
		#ifdef DEBUG
		lflevel2 = fopen(LOGFILE,"a");
		fprintf(lflevel2,"PTP - insert into bitmap OK\n");
		fclose(lflevel2);
		#endif
		image = NULL;
		delete(file);
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
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//height = (*params).objectinfo[0].ImagePixHeight;
	height = (*params).objectinfo[currentpicturenumber].ImagePixHeight;
	//ptp_closesession(params);
	return(B_NO_ERROR);
}

status_t getImageWidth(int &width)
{
	#ifdef DEBUG
	lflevel2 = fopen(LOGFILE,"a");	
	fprintf(lflevel2,"PTP - Get image width\n");
	fclose(lflevel2);
	#endif
	//ptp_opensession(params,1);
	//(*params).objectinfo =(PTPObjectInfo*)malloc(sizeof(PTPObjectInfo));
	//ptp_getobjectinfo(params,currentitemhandle,&params->objectinfo[0]);
	//width = (*params).objectinfo[0].ImagePixWidth;
	width = (*params).objectinfo[currentpicturenumber].ImagePixWidth;
	//ptp_closesession(params);
	return(B_NO_ERROR);
}

status_t getDeviceType(int &type)
{
	#ifdef DEBUG
	lflevel2 = fopen(LOGFILE,"a");	
	fprintf(lflevel2,"PTP - Get get the type of device\n");
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
