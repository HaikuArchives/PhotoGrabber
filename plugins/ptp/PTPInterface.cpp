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
#include <string.h>
#include "PTPInterface.h"
#include "preferences.h"
#include "debug.h"

int globalEndpoint;
short verbose = 0;
unsigned int USBBULK_BUFFER = 1024;
FILE *lfptpi;

// initialize a PTP camera via the USB protocol
//
bool  PTP_init_ptp_usb(PTPParams* params,BUSBDevice *dev)
{
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,"PTP: Init ptp usb\n");
		fclose(lfptpi);
	#endif
	LoadSettingsFromFile();
	params->write_func=PTP_ptp_write_func;
	params->read_func=PTP_ptp_read_func;
	params->error_func=PTP_error;
	params->debug_func=PTP_debug;
	params->sendreq_func=ptp_usb_sendreq;
	params->senddata_func=ptp_usb_senddata;
	params->getresp_func=ptp_usb_getresp;
	params->getdata_func=ptp_usb_getdata;
	params->data=dev;	
	params->transaction_id=0;
	params->byteorder = PTP_DL_LE;
	
	return B_OK;
}

// read function for the PTP camera
//
short
PTP_ptp_read_func (unsigned char *bytes, unsigned int size, void *data)
{
	int result,endpoint=0,interface=0, ret = 0;
	BUSBDevice *dev=(BUSBDevice *)data;
	const BUSBEndpoint 	*iept;
	
	PTP_logValue(PTP_ENDPOINT_READ,endpoint);
	PTP_logValue(PTP_INTERFACE_READ,interface);
	iept=dev->ActiveConfiguration()->InterfaceAt(interface)->EndpointAt(endpoint);	
	if(iept->IsOutput())
	{
		endpoint = 1;
		iept=dev->ActiveConfiguration()->InterfaceAt(interface)->EndpointAt(endpoint);
	}
	
	if(iept == NULL)
	{
		PTP_logValue(PTP_ERROR_ENDPOINT_NULL,0);
		return PTP_logError(PTP_ERROR_IO_READ);
	}
	
	if(size <= USBBULK_BUFFER)
		result = iept->BulkTransfer(bytes,size);
	else
	{
		/* for pictures bigger then 512k we need another way of using the buklktransfer
	   		otherwise we will get an error and the pictures can't be downloaded
		*/
		unsigned int 	length;
		unsigned char 	*buf;
		unsigned int	index;
	
		buf = new unsigned char[USBBULK_BUFFER];
		#ifdef DEBUG
			lfptpi = fopen(LOGFILE,"a");
			fprintf(lfptpi,"PTP - Total file size: %d k.\n",size);
			fclose(lfptpi);
		#endif
		length = size;
		index = 0;
		while(length > 0)
		{
			result = iept->BulkTransfer(buf,USBBULK_BUFFER);
			#ifdef DEBUG
				lfptpi = fopen(LOGFILE,"a");
				fprintf(lfptpi,"PTP - Transfered %d k.\n",(int)result);
				fclose(lfptpi);
			#endif
			if(result > 0)
			{
				unsigned int i = 0;
				while(i < result && index < size)
				{
					*bytes = buf[i];
					bytes++;
					i++;index++;				
				}
				//if(length > USBBULK_BUFFER)
				//	length -= USBBULK_BUFFER;
				//else
				//	length = 0;
				length -= result;
				delete(buf);
				buf = new unsigned char[USBBULK_BUFFER];
				#ifdef DEBUG
					lfptpi = fopen(LOGFILE,"a");
					fprintf(lfptpi,"PTP - Still %d k to go\n",length);
					fclose(lfptpi);
				#endif
			}
			else
			{
				#ifdef DEBUG
					lfptpi = fopen(LOGFILE,"a");
					fprintf(lfptpi,"PTP: ERROR in transfer, please check the camera.\n");
					fclose(lfptpi);
				#endif
				delete(buf);
				return(PTP_ERROR_IO_READ);
			}
		}
		delete(buf);
		PTP_logValue(PTP_BULK_READ_SIZE,(int)size);
		return PTP_logError(PTP_RC_READ_OK);	
	}
		
	if (result >= 0)
	{
		PTP_logValue(PTP_BULK_READ_SIZE,(int)size);
		return PTP_logError(PTP_RC_READ_OK);
	}
	else 
	{
		PTP_logValue(PTP_BULK_READ_SIZE,(int)size);
		return PTP_logError(PTP_ERROR_IO_READ);
	}
}

// write function for the PTP camera
//
short
PTP_ptp_write_func (unsigned char *bytes, unsigned int size, void *data)
{
	int result,endpoint=0,interface=0, ret = 0;
	BUSBDevice *dev=(BUSBDevice *)data;
	const BUSBEndpoint 	*iept;

	PTP_logValue(PTP_ENDPOINT_WRITE,endpoint);
	PTP_logValue(PTP_INTERFACE_WRITE,interface);
	iept=dev->ActiveConfiguration()->InterfaceAt(interface)->EndpointAt(endpoint);
	if(iept->IsInput())
	{
		endpoint = 1;
		iept=dev->ActiveConfiguration()->InterfaceAt(interface)->EndpointAt(endpoint);
	}
	
	if(iept == NULL)
		return PTP_logError(PTP_ERROR_IO_WRITE);

	result= iept->BulkTransfer(bytes,size);
	if (result >= 0)
		return PTP_logError(PTP_RC_WRITE_OK);
	else 
		return PTP_logError(PTP_ERROR_IO_WRITE);
}

// debug function
//
void
PTP_debug (void *data, const char *format, va_list args)
{
	if (verbose<2) return;
	vfprintf (stderr, format, args);
	fprintf (stderr,"\n");
	fflush(stderr);
}

//error function
//
void
PTP_error (void *data, const char *format, va_list args)
{
	if (!verbose) return;
	vfprintf (stderr, format, args);
	fprintf (stderr,"\n");
	fflush(stderr);
}

// log function
//
int 
PTP_logError(int ErrorMes)
{
	char *errorMessage;
	
	switch(ErrorMes)
	{
		case PTP_ERROR_IO_READ:
			errorMessage = "PTP:Error reading bulktransfer\n";
			ErrorMes = PTP_ERROR_IO;
			break;
		case PTP_ERROR_IO_WRITE:
			errorMessage = "PTP:Error writing bulktransfer\n";
			ErrorMes = PTP_ERROR_IO;
			break;
		case PTP_ERROR_ENDPOINT_READ:
			errorMessage = "PTP:Error reading from endpoint\n";
			ErrorMes = PTP_ERROR_IO;
			break;
		case PTP_ERROR_ENDPOINT_WRITE:
			errorMessage = "PTP:Error writing to endpoint\n";
			ErrorMes = PTP_ERROR_IO;
			break;
		case PTP_RC_READ_OK:
			errorMessage = "PTP: Reading bulktransfer OK\n";
			ErrorMes = PTP_RC_OK;
			break;
		case PTP_RC_WRITE_OK:
			errorMessage = "PTP: Writing bulktransfer OK\n";
			ErrorMes = PTP_RC_OK;
			break;
		default:
			errorMessage = "PTP: An unexpected error occured\n";
	}
	#ifdef DEBUG
		lfptpi = fopen(LOGFILE,"a");
		fprintf(lfptpi,errorMessage);
		fclose(lfptpi);
	#endif
	return(ErrorMes);
}

void
PTP_logValue(int ValueMes, int Value)
{
	#ifdef DEBUG
	char *valueMessage;
	
	switch(ValueMes)
	{
		case PTP_ENDPOINT_READ:
			valueMessage = "PTP: The read endpoint is: %d\n";
			break;
		case PTP_ENDPOINT_WRITE:
			valueMessage = "PTP: The write endpoint is: %d\n";
			break;
		case PTP_INTERFACE_READ:
			valueMessage = "PTP: The read interface is: %d\n";
			break;
		case PTP_INTERFACE_WRITE:
			valueMessage = "PTP: The write interface is: %d\n";
			break;
		case PTP_BULK_READ_SIZE:
			valueMessage = "PTP: The bulk read size is: %d\n";
			break;
		case PTP_ERROR_ENDPOINT_NULL:
			valueMessage = "PTP: The endpoint is null\n";
			break;
		default:
			valueMessage = "PTP: An unexpected error occured\n";
	}
	lfptpi = fopen(LOGFILE,"a");
	fprintf(lfptpi,valueMessage,Value);
	fclose(lfptpi);
	#endif
}
