/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
// 
// Includes
#include "MSDScsiProbe.h"

//
// Probe and mount the correct SCSI device
char *SCSIProbeMount(const char *device_path, BUSBDevice *usbdev)
{
  int fd,e;
  int path,targ,lun,type;
  raw_device_command rdc;
  uchar data[36], sense[16];
  char 	volume_name[B_FILE_NAME_LENGTH];
  char	mount_point[B_FILE_NAME_LENGTH];
  char	tmp[B_FILE_NAME_LENGTH];
  int	index = 1;
  BDirectory directory;

  if(!strncmp("/dev/disk/scsi/",device_path,14))
    sscanf(device_path,"/dev/disk/scsi/%d/%d/%d/raw",&path,&targ,&lun); 
  else 
    return "";
  // fill out our raw device command data
  rdc.data = data;
  rdc.data_length = 36;
  rdc.sense_data = sense;
  rdc.sense_data_length = 0;
  rdc.timeout = 1000000;
  rdc.flags = B_RAW_DEVICE_DATA_IN;
  rdc.command_length = 6;
  rdc.command[0] = 0x12;
  rdc.command[1] = 0x00;
  rdc.command[2] = 0x00;
  rdc.command[3] = 0x00;
  rdc.command[4] = 36;
  rdc.command[5] = 0x00;
  // Open the raw device
  if((fd = open(device_path,0)) < 0) 
  	return "";
  e = ioctl(fd, B_RAW_DEVICE_COMMAND, &rdc, sizeof(rdc));
  close(fd);
  if((e != 0) || (rdc.cam_status != CAM_REQ_CMP)) 
  	return "";
  // Check if the Manufacturer string of the usb device is the same as the 
  // one of the scsi device. If so, try to mount the device. Once mounted,
  // it should be possible to get the pictures.
  char *scsi_manufac = new char[7];
  strncpy(scsi_manufac,(char*)data+8,7);
  int i = 6;
  while(i >= 0)
  {
  	if(scsi_manufac[i] == ' ')
  		scsi_manufac[i] = '\0';
  	else
  		break;
  	i--;
  }
  char *usb_manufac	= new char[7];
  strncpy(usb_manufac,usbdev->ManufacturerString(),7);
  if(!strncmp(scsi_manufac,usb_manufac,7))
  {
  	type = data[0] & 0x1F;
  	sprintf(volume_name,usb_manufac);
  	//TODO:: check the type of the scsi-device and the usb-device
  	//
  	// if there is no volume name, use a default
  	if (!strlen(volume_name))
    	return "";
  	else 
  	{
    	// create mount point at root
    	sprintf(tmp, "/%s",volume_name);
    	// convert any forward slashes to ':'
    	//for (loop = 1;loop < strlen(volume_name);loop++)
      		//if (tmp[loop] == '/')
        		//tmp[loop] = ':';
     }
  	// make sure name is unique or if not unique,
  	// make sure existing directory is empty 
  	strcpy(mount_point, tmp);
  	while (1) 
  	{
    	if (mkdir(mount_point, 0777) >= 0)
      		break;
    	directory.SetTo(mount_point);
    	if ((!directory.InitCheck()) &&(!directory.CountEntries()))
      		break;
    sprintf(mount_point, "%s%d", tmp, index++);
  	}
  	// try mounting 
  	if (mount("dos", mount_point,device_path, 0, NULL, 0) < 0) 
   		return "";
  	return "disk";
  }
  else
  	return "";
}

//
// Unmount the correct SCSI device
bool SCSIUnmount(char *usbDeviceName)
{
	// try unmounting 
  	if (unmount(usbDeviceName) < 0) 
   		return false;
  	return true;
}
//
// Recursively wander down from a path, looking for "raw"
// devices to call inquiry on and return the the mount name
char *SCSIMount(const char *path,BUSBDevice *device)
{
  BDirectory dir(path);
  if(dir.InitCheck() == B_OK)
  {
    BEntry entry;
    while(dir.GetNextEntry(&entry,true) >= 0) 
    {
      BPath name;
      entry.GetPath(&name);
      if(entry.IsDirectory()) 
      {
        SCSIMount(name.Path(),device);
      } 
      else if(!strcmp(name.Leaf(),"raw"))
        if(SCSIProbeMount(name.Path(),device) == "")
        	return "disk";
    }
    return "";
  }
  else
  	return "";
}
