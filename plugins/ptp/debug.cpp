/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
#include "debug.h"
#include "global.h"
#include <Path.h>
#include <FindDirectory.h>
#include <fstream.h>
#include <cstdio>
#include <iostream.h>
#include <String.h>
#include <Looper.h>

const int32 MSG_PLUGINDEBUG			= 0x01012000;

extern BLooper* msgtarget;
FILE *lfdebug;

void debug(char* debugstring)
{
	sendmessage(debugstring);
}

void debug(BString debugstring)
{
	sendmessage((char *)debugstring.String());
}

void debug(BString debugstring1, BString debugstring2)
{
	char* stringtosend = new char[debugstring1.Length() + debugstring2.Length() + 3];
	sprintf(stringtosend, "%s %s",debugstring1.String(),debugstring2.String());
	sendmessage(stringtosend);
}

void debug(BString debugstring1, int debugval2)
{
	char* stringtosend = new char[debugstring1.Length() + 30];
	sprintf(stringtosend, "%s %d",debugstring1.String(),debugval2);
	sendmessage(stringtosend);
}

void sendmessage(char* message)
{
	if (msgtarget != NULL)
	{
		BMessage *msg;
		msg = new BMessage(MSG_PLUGINDEBUG);
		msg->AddString("debugmsg", message);
		msg->AddString("plugname", "libbdcp_PTP");
		msgtarget->PostMessage(msg);
		delete msg;
	}
}

int logError(int ErrorMes)
{
	#ifdef DEBUG
	char *errorMessage;
	switch(ErrorMes)
	{
		case PTPCAM_DEV_NO_FIND:
			errorMessage = "CAM: No PTP device is present\n";	
			break;
		case PTPCAM_INIT_FAIL:
			errorMessage = "CAM: Initialisation of the PTP device failed\n";
			break;
		case PTPCAM_NO_HANDLES:
			errorMessage = "CAM: There are no handles\n";
			break;
		case PTPCAM_GETPIC_FAIL:
			errorMessage = "CAM: Could not get pictures\n";
			break;
		case PTPCAM_FILE_EXISTS:
			errorMessage = "CAM: File already exists\n";
			break;
		case PTPCAM_FILECREATE_FAIL:
			errorMessage = "CAM: Could not create file\n";
			break;
		case PTPCAM_VOLUME_FULL:
			errorMessage = "CAM: The volume is full\n";
			break;
		case PTPCAM_FILEWRITE_FAIL:
			errorMessage = "CAM: Could not write to a file\n";
			break;
		case PTPCAM_TRANSFER_FAIL:
			errorMessage = "CAM:The transfer failed\n";
			break;
		case PTPCAM_DEL_PIC_FAIL:
			errorMessage = "CAM:Delete picture failed\n";
			break;
		case PTPCAM_GET_NAME_FAIL:
			errorMessage = "CAM:Could not get picture name\n";
			break;
		case PTPCAM_NEW_FILE_FAIL:
			errorMessage = "CAM:Could not create a file to save the thumbnail\n";
			break;
		default:
			errorMessage = "CAM:An unexpected error occured\n";
	}
	lfdebug = fopen(LOGFILE,"a");
	fprintf(lfdebug,errorMessage);
	fclose(lfdebug);
	#endif
	return(ErrorMes);
}
