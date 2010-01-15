/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// File defenition
#ifndef	PTPINTF_H
#define PTPINTF_H
//
// Include
#include <USBKit.h>
#include "global.h"
extern "C" {
#include "libptp/ptp.h"
}
//
typedef struct  {
	BUSBDevice 	*device;
	uint32		configuration;
	uint32		interface;
	uint32		interruptInput;
	uint32		bulkInput;
	uint32		bulkOutput;
	char*		location;
} USBCameraDevice;
//

uint16_t 			ptp_init_usb (PTPParams* params);
uint16_t 			ptp_exit_usb (PTPParams* params);
int					ptp_read_func (unsigned char *bytes, unsigned int size, void *data);
int		 			ptp_write_func (unsigned char *bytes, unsigned int size, void *data);
int					ptp_check_int(unsigned char *bytes, unsigned int size, void *data);
int 				ptp_clear_read_halt(void *data, int endpoint);
void 				ptp_debug (void *data, const char *format, va_list args);
void				ptp_error (void *data, const char *format, va_list args);

#define PTP_OFFSET							255

#endif


