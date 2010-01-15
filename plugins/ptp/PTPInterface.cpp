/*
****************************************************************
* Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PTPInterface.h"
#include "preferences.h"
#include "debug.h"
#include "results.h"

int globalEndpoint;
FILE *lfptpi;
//
// initialize a PTP camera via the USB protocol
uint16_t  ptp_init_usb(PTPParams* params)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Init ptp usb\n");
		fclose(lfptpi);
	#endif
	
	params->error_func		= ptp_error;
	params->debug_func		= ptp_debug;
	params->sendreq_func	= ptp_usb_sendreq;
	params->senddata_func	= ptp_usb_senddata;
	params->getresp_func	= ptp_usb_getresp;
	params->getdata_func	= ptp_usb_getdata;
	params->event_wait		= ptp_usb_event_wait;
	params->event_check		= ptp_usb_event_check;
	params->cancelreq_func	= ptp_usb_control_cancel_request;
	params->transaction_id	= 0;
	params->byteorder 		= PTP_DL_LE;
	// Open a session
	ptp_opensession(params,1);	
							
							
	return PG_OK;
}
//
//	Exit the PTP USB Device
uint16_t ptp_exit_usb(PTPParams* params)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Exit ptp usb\n");
		fclose(lfptpi);
	#endif
	
	// Close the session and free the PTP parameters
	ptp_closesession(params);
	ptp_free_params (params);
	
	return PG_OK;

}
//
//	Read from USB
int 
ptp_read_func (unsigned char *bytes, unsigned int size, void *data)
{
	int result;
	USBCameraDevice *cameraDevice = (USBCameraDevice *)data;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	const BUSBEndpoint 	*iept;
	
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Read from endpoint %d\n",cameraDevice->bulkInput);
		fclose(lfptpi);
	#endif
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkInput);	
	
	if(iept == NULL)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - The endpoint is null\n");
			fclose(lfptpi);
		#endif
		return PG_ERROR_IO_READ;
	}
	
	result = iept->BulkTransfer(bytes,size);
	
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - The bulk read size is: %d k.\n",(int)result);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - Bulk read failed.\n");
			fclose(lfptpi);
		#endif
		return PG_ERROR_IO_READ;
	}
}
//
//	Write to USB
int 
ptp_write_func (unsigned char *bytes, unsigned int size, void *data)
{
	int result;
	USBCameraDevice *cameraDevice = (USBCameraDevice *)data;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	const BUSBEndpoint 	*iept;

	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Write at endpoint %d\n",cameraDevice->bulkOutput);
		fclose(lfptpi);
	#endif
	iept=dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkOutput);
	
	if(iept == NULL)
		return PG_ERROR_IO_WRITE;

	result = iept->BulkTransfer(bytes,size);
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - The bulk write size is: %d k.\n",(int)size);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - Bulk write failed.\n");
			fclose(lfptpi);
		#endif
		return PG_ERROR_IO_WRITE;
	}
}
//
//	Check interupt
int
ptp_check_int(unsigned char *bytes, unsigned int size, void *data)
{
	int result;
	USBCameraDevice *cameraDevice = (USBCameraDevice *)data;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	const BUSBEndpoint 	*iept;

	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Check interrupt USB\n");
		fclose(lfptpi);
	#endif
	
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->interruptInput);
		
	if(iept == NULL)
		return PG_ERROR_IO_READ;
		
	result= iept->InterruptTransfer(bytes,size);
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - The interrupt write size is: %d k.\n",(int)size);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP Interface - Interrupt write failed.\n");
			fclose(lfptpi);
		#endif
		return PG_ERROR_IO_READ;
	}
}
//
//	Get the timeout
int
ptp_clear_read_halt(void *data, int endpoint)
{
	USBCameraDevice *cameraDevice = (USBCameraDevice *)data;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	const BUSBEndpoint 	*iept;

	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP Interface - Write to USB\n");
		fclose(lfptpi);
	#endif
	switch(endpoint) {
		case PG_USB_ENDPOINT_IN:
			iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkInput);
			break;
		case PG_USB_ENDPOINT_OUT:
			iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkOutput);
			break;
		case PG_USB_ENDPOINT_INT:
			iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->interruptInput);
			break;
	}
	
	if(iept == NULL)
		return PG_ERROR_IO_USB_CLEAR_HALT;
		
	if( iept->IsStalled())
		iept->ClearStall();

	return PG_OK;
}

//
// debug function
void
ptp_debug (void *data, const char *format, va_list args)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		vfprintf (lfptpi, format, args);
		fprintf(lfptpi,"\n");
		fclose(lfptpi);
	#endif	
}

//error function
//
void
ptp_error (void *data, const char *format, va_list args)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		vfprintf (lfptpi, format, args);
		fprintf(lfptpi,"\n");
		fclose(lfptpi);
	#endif
}
