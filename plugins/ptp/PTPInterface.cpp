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

int globalEndpoint;
FILE *lfptpi;
//
// initialize a PTP camera via the USB protocol
uint16_t  ptp_init_usb(PTPParams* params, USBCameraDevice *cameraDevice)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP - Init ptp usb\n");
		fclose(lfptpi);
	#endif
	// Initialize the USB device
	uint16_t ret = PTP_ERROR_IO;
	uint32 i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	
	for(i=0;i<(int)dev->CountConfigurations();i++)
	{
		conf = dev->ConfigurationAt(i);
		if(conf)
		{
			for(j=0;j<(int)conf->CountInterfaces();j++)
			{
				ifc = conf->InterfaceAt(j);	
				if(ifc)
					if(ifc->Class() == 6)
					{
						#ifdef DEBUG
							lfptpi = fopen(LOGFILE,"a");
							fprintf(lfptpi,"PTP - Device %s found and attached!\n",dev->ProductString());
							fclose(lfptpi);
						#endif
						
						if(dev->InitCheck() || dev->SetConfiguration(dev->ConfigurationAt(i)))
							logError(PTPCAM_DEV_NO_FIND);
						else
						{
							// initialize the ptp device
							LoadSettingsFromFile();
							//	Save the configuration, interface and endpoints for later use
							cameraDevice->configuration = i;
							cameraDevice->interface = j;
							uint32 k = ifc->CountEndpoints();
							while (k > 0) 
							{
								k--;
								if(ifc->EndpointAt(k)->IsBulk())
								{
									if(ifc->EndpointAt(k)->IsInput())
									{
										cameraDevice->bulkInput = k;
										params->maxpacketsize = ifc->EndpointAt(k)->MaxPacketSize();
									}
									else 
										cameraDevice->bulkOutput = k;
								}
								else if(ifc->EndpointAt(k)->IsInterrupt())
									cameraDevice->interruptInput = k;
								
							};
							//
							params->error_func		= ptp_error;
							params->debug_func		= ptp_debug;
							params->sendreq_func	= ptp_usb_sendreq;
							params->senddata_func	= ptp_usb_senddata;
							params->getresp_func	= ptp_usb_getresp;
							params->getdata_func	= ptp_usb_getdata;
							params->event_wait		= ptp_usb_event_wait;
							params->event_check		= ptp_usb_event_check;
							params->cancelreq_func	= ptp_usb_control_cancel_request;
							params->data			= cameraDevice;	
							params->transaction_id	= 0;
							params->byteorder 		= PTP_DL_LE;
							ret = PTP_RC_OK;	
						}
					}	
			}
		}
	}
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP - Using configuration %d!\n",cameraDevice->configuration);
		fclose(lfptpi);
	#endif
	return ret;
}
//
//	Exit the PTP USB Device
uint16_t ptp_exit_usb(PTPParams* params, BUSBDevice *device)
{
	// Initialize the USB device
	int i,j;
	const BUSBConfiguration *conf;
	const BUSBInterface *ifc;
	USBCameraDevice *cameraDevice = (USBCameraDevice *)(params->data);
	
	if(cameraDevice->device == device)
		return PTP_RC_OK;
	
	return PTP_ERROR_IO;
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
		fprintf(lfptpi,"PTP - Read from endpoint %d\n",cameraDevice->bulkInput);
		fclose(lfptpi);
	#endif
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkInput);	
	
	if(iept == NULL)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - The endpoint is null\n");
			fclose(lfptpi);
		#endif
		return PTP_ERROR_IO;
	}
	
	result = iept->BulkTransfer(bytes,size);
	
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - The bulk read size is: %d k.\n",(int)result);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - Bulk read failed.\n");
			fclose(lfptpi);
		#endif
		return PTP_ERROR_IO;
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
		fprintf(lfptpi,"PTP - Write at endpoint %d\n",cameraDevice->bulkOutput);
		fclose(lfptpi);
	#endif
	iept=dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->bulkOutput);
	
	if(iept == NULL)
		return PTP_ERROR_IO;

	result = iept->BulkTransfer(bytes,size);
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - The bulk write size is: %d k.\n",(int)size);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - Bulk write failed.\n");
			fclose(lfptpi);
		#endif
		return PTP_ERROR_IO;
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
		fprintf(lfptpi,"PTP - Check interrupt USB\n");
		fclose(lfptpi);
	#endif
	
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->interruptInput);
		
	if(iept == NULL)
		return PTP_ERROR_IO;
		
	result= iept->InterruptTransfer(bytes,size);
	if (result >= 0)
	{
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - The interrupt write size is: %d k.\n",(int)size);
			fclose(lfptpi);
		#endif
		return result;
	}
	else
	{ 
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - Interrupt write failed.\n");
			fclose(lfptpi);
		#endif
		return PTP_ERROR_IO;
	}
}
//
//	Get the timeout
int
ptp_clear_read_halt(void *data)
{
	USBCameraDevice *cameraDevice = (USBCameraDevice *)data;
	BUSBDevice *dev = (BUSBDevice *)(cameraDevice->device);
	const BUSBEndpoint 	*iept;

	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP - Write to USB\n");
		fclose(lfptpi);
	#endif
	
	iept = dev->ActiveConfiguration()->InterfaceAt(cameraDevice->interface)->EndpointAt(cameraDevice->interruptInput);
	
	if(iept == NULL)
		return PTP_ERROR_IO;
		
	if( iept->IsStalled())
		iept->ClearStall();

	return PTP_RC_OK;
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
