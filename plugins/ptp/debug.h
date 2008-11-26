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
#define LOGFILE "/boot/var/tmp/ptp.log"
// error messages
#define	PTPCAM_OFFSET				155
#define PTPCAM_DEV_NO_FIND			PTPCAM_OFFSET + 1
#define PTPCAM_INIT_FAIL			PTPCAM_OFFSET + 2
#define PTPCAM_NO_HANDLES			PTPCAM_OFFSET + 3
#define PTPCAM_GETPIC_FAIL			PTPCAM_OFFSET + 4
#define PTPCAM_FILE_EXISTS			PTPCAM_OFFSET + 5
#define PTPCAM_FILECREATE_FAIL		PTPCAM_OFFSET + 6
#define PTPCAM_VOLUME_FULL			PTPCAM_OFFSET + 7
#define PTPCAM_FILEWRITE_FAIL		PTPCAM_OFFSET + 8
#define PTPCAM_TRANSFER_FAIL		PTPCAM_OFFSET + 9
#define PTPCAM_DEL_PIC_FAIL			PTPCAM_OFFSET + 10
#define PTPCAM_GET_NAME_FAIL		PTPCAM_OFFSET + 11
#define PTPCAM_NEW_FILE_FAIL		PTPCAM_OFFSET + 12

#endif
