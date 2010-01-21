/*
****************************************************************
* Copyright (c) 2004-2007,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/ 
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <String.h>

void sendmessage(char* message);
int logError(int ErrorMes);
void debug(char* debugstring);
void debug(BString debugstring);
void debug(BString debugstring1, BString debugstring2);
void debug(BString debugstring1, int debugval2);
// defenitions
//#define	DEBUG
// log-file
#define LOGFILE "/boot/home/Desktop/msd.log"
// error messages
#define	MSCAM_OFFSET				155
#define MSCAM_DEV_NO_FIND			MSCAM_OFFSET + 1
#define MSCAM_INIT_FAIL				MSCAM_OFFSET + 2
#define MSCAM_GETPIC_FAIL			MSCAM_OFFSET + 4
#define MSCAM_FILE_EXISTS			MSCAM_OFFSET + 5
#define MSCAM_FILECREATE_FAIL		MSCAM_OFFSET + 6
#define MSCAM_VOLUME_FULL			MSCAM_OFFSET + 7
#define MSCAM_FILEWRITE_FAIL		MSCAM_OFFSET + 8
#define MSCAM_TRANSFER_FAIL			MSCAM_OFFSET + 9
#define MSCAM_DEL_PIC_FAIL			MSCAM_OFFSET + 10
#define MSCAM_GET_NAME_FAIL			MSCAM_OFFSET + 11
#define MSCAM_NEW_FILE_FAIL			MSCAM_OFFSET + 12

#endif
