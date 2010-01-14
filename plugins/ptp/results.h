/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/

// Results

#define PG_OK						0
#define PG_ERROR					-1
#define PG_ERROR_IO_READ			-34
#define PG_ERROR_IO_WRITE			-35
#define PG_ERROR_IO_USB_CLEAR_HALT 	-51

enum {
	PG_USB_ENDPOINT_IN,
	PG_USB_ENDPOINT_OUT,
	PG_USB_ENDPOINT_INT,
};
