/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
// 
//		Includes
#include <stdio.h>
//		Local includes

#include "MSDItem.h"
//
//	MSD_Item::MSD Item
MSD_Item::MSD_Item(const char *name, uint32 size, char *date, uint32 Xres, uint32 Yres)
{
	if(name != "")
		MSDItemName = name;
	else
		MSDItemName = "no name";
	MSDItemSize=size;
	if(date != "")
		MSDItemDate=date;
	else
		MSDItemDate = "01/01/2001";
	MSDItemXres=Xres;
	MSDItemYres=Yres;
}
//
//		MSD_Item::MSD Item constructor
MSD_Item::MSD_Item()
{
	//nothing yet
}
//
//		MSD_Item::Item destructor
MSD_Item::~MSD_Item()
{
	//nothing yet
}
//
//	MSD_Item::Get Name
const char* MSD_Item::GetName()
{
	return MSDItemName.String();
}
//
//	MSD_Item::Get Item
const char* MSD_Item::GetItemPath()
{
	return MSDItemPath.String();
}
//
//	MSD_Item::Get Date
const char* MSD_Item::GetDate()
{
	return MSDItemDate.String();
}
//
//	MSD_Item::Get Handle
uint32 MSD_Item::GetHandle()
{
	return MSDItemHandle;
}
//
//	MSD_Item::Get Name
uint32 MSD_Item::GetSize()
{
	return MSDItemSize;
}
//
//	MSD_Item::Get Name
uint32 MSD_Item::GetXRes()
{
	return MSDItemXres;
}
//
//	MSD_Item::Get Name
uint32 MSD_Item::GetYRes()
{
	return MSDItemYres;
}
//
//	MSD_Item::set Handle
void MSD_Item::setHandle(uint32 handle)
{
	MSDItemHandle=handle;
}
//
//	MSD_Item::set Item Path
void MSD_Item::setItemPath(const char *path)
{
	if(path != "")
		MSDItemPath = path;
}
//
//	MSD_Item::set Name
void MSD_Item::setName(const char *name)
{
	if(name != "")
		MSDItemName = name;
}
//
//	MSD_Item::set Date
void MSD_Item::setDate(char *date)
{
	if(date != "")
		MSDItemDate=date;
}
//
//	MSD_Item::set Size
void MSD_Item::setSize(uint32 size)
{
	MSDItemSize=size;
}
//
//	MSD_Item::set Resolution
void MSD_Item::setRes(uint32 Xres, uint32 Yres)
{
	MSDItemXres=Xres;
	MSDItemYres=Yres;
}
//
//	MSD_Item::set X-Resolution
void MSD_Item::setXRes(uint32 Xres)
{
	MSDItemXres=Xres;
}
//
//	MSD_Item::set Y-Resolution
void MSD_Item::setYRes(uint32 Yres)
{
	MSDItemYres=Yres;
}


