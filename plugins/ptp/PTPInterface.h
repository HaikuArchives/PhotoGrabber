/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
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


//int					PTP_findendpoint(int type, int &interface, int &endpoint);
bool 				PTP_init_ptp_usb(PTPParams* params, BUSBDevice* dev);
short		 		PTP_ptp_read_func (unsigned char *bytes, unsigned int size, void *data);
short		 		PTP_ptp_write_func (unsigned char *bytes, unsigned int size, void *data);
void 				PTP_debug (void *data, const char *format, va_list args);
void				PTP_error (void *data, const char *format, va_list args);
int 				PTP_logError(int ErrorMes);
void				PTP_logValue(int ValueMes, int Value);

#define PTP_OFFSET							255
#define PTP_ERROR_IO_READ					PTP_OFFSET + 1
#define PTP_ERROR_IO_WRITE					PTP_OFFSET + 2
#define PTP_ERROR_ENDPOINT_READ				PTP_OFFSET + 3
#define PTP_ERROR_ENDPOINT_WRITE			PTP_OFFSET + 4
#define PTP_RC_READ_OK						PTP_OFFSET + 5
#define PTP_RC_WRITE_OK						PTP_OFFSET + 6
#define PTP_ENDPOINT_READ					PTP_OFFSET + 7
#define PTP_ENDPOINT_WRITE					PTP_OFFSET + 8
#define PTP_INTERFACE_READ					PTP_OFFSET + 9
#define PTP_INTERFACE_WRITE					PTP_OFFSET + 10
#define	PTP_BULK_READ_SIZE					PTP_OFFSET + 11
#define PTP_ERROR_ENDPOINT_NULL				PTP_OFFSET + 12
#define PTP_ENDPOINT_INPUT					PTP_OFFSET + 13
#define PTP_ENDPOINT_OUTPUT					PTP_OFFSET + 14

#endif


