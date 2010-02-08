/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Local Includes
#include "PTPInterface.h"
#include "logger.h"
#include "results.h"

int globalEndpoint;
//
// initialize a PTP camera via the USB protocol
uint16_t  ptp_init_usb(PTPParams* params)
{
	LogDebug("PTP - Init ptp usb.");
	
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
	LogDebug("PTP - Exit ptp usb.");
	
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
	
	LogDebug("PTP - Read from endpoint %d.",cameraDevice->bulkInput);

	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkInput);	
	
	if(iept == NULL)
	{
		LogDebug("PTP - The endpoint is null.");

		return PG_ERROR_IO_READ;
	}
	
	result = iept->BulkTransfer(bytes,size);
	
	if (result >= 0)
	{
		LogDebug("PTP - The bulk read size is: %d k.",(int)result);

		return result;
	}
	else
	{ 
		LogDebug("PTP - Bulk read failed.");

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

	LogDebug("PTP - Write at endpoint %d.",cameraDevice->bulkOutput);

	iept=dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkOutput);
	
	if(iept == NULL)
		return PG_ERROR_IO_WRITE;

	result = iept->BulkTransfer(bytes,size);
	if (result >= 0)
	{
		LogDebug("PTP - The bulk write size is: %d k.",(int)size);

		return result;
	}
	else
	{ 
		LogDebug("PTP - Bulk write failed.");

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

	LogDebug("PTP - Check interrupt USB.");

	
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->interruptInput);
		
	if(iept == NULL)
		return PG_ERROR_IO_READ;
		
	result= iept->InterruptTransfer(bytes,size);
	if (result >= 0)
	{
		LogDebug("PTP - The interrupt write size is: %d k.",(int)size);
		return result;
	}
	else
	{ 
		LogDebug("PTP - Interrupt write failed.");
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

	LogDebug("PTP - Write to USB.");

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
	LogDebug(format, args);	
}

//error function
//
void
ptp_error (void *data, const char *format, va_list args)
{
	LogDebug(format, args);
}
