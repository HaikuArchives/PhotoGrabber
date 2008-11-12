/*****************************************************************
 * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 //
//		Local Includes
#include "MSInterface.h"
#include "global.h"
#include "debug.h"

//
// 		Includes
#include <Path.h>
#include <FindDirectory.h>
#include <View.h>
#include <Volume.h>

//
// External variables
int READ_BUFFER  = 1024;
//
//		MSDInterface constructor
MSDInterface::MSDInterface(BUSBDevice *dev) 
{	
	//   initialization
	msdDeviceName = new char[8];
	msdVersion = new char[4];
	msdMountPoint = new char[B_FILE_NAME_LENGTH];
	camConnected = false;
	numberOfItems = 0;
}
MSDInterface::~MSDInterface()
{	
	// Nothing yet
}
//
//		MSDInterface: MSD_logError
int MSDInterface::getNumberOfItems()
{	
	return numberOfItems;
}
//
//		MSDInterface: setCurrentItem
bool MSDInterface::setCurrentItem(int index)
{
	return false;
}
//
//		MSDInterface: Download picture
bool MSDInterface::downloadItem(int index,BPath path)
{
	MSD_Item *item;
	item = getMSDItem(index);
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,"Donwload file: %s to %s\n",item->GetItemPath(),path.Path());
		fclose(file);
	#endif
	BFile *fromfile = new BFile(item->GetItemPath(), B_READ_ONLY);
	BFile *tofile;
	BNodeInfo *ni;
	#ifdef DEBUG
		file = fopen(LOGFILE,"a");
		fprintf(file,"Create tofilename");
		fclose(file);
	#endif
	char filename[B_FILE_NAME_LENGTH];
	strcpy(filename,path.Path());
	strcat(filename,"/");
	strcat(filename,item->GetName());
	#ifdef DEBUG
		file = fopen(LOGFILE,"a");
		fprintf(file,"tofilename created");
		fclose(file);
	#endif 
	if((tofile=new BFile(filename, B_WRITE_ONLY | B_CREATE_FILE )))
	{
		long int fherr;

		if((fherr = tofile->InitCheck()) != B_OK)
			return(B_ERROR);
		if((ni=new BNodeInfo(tofile)))
		{
			ni->SetType("image/jpeg");
			delete ni;
		}
		// read the data from the fromfile and write it to the tofile
		char buf[READ_BUFFER];
		int32 size = item->GetSize();
		int32 remainsize = size;
		int32 writesize = 0;
		while(remainsize > 0)
		{
			writesize = fromfile->Read((void *)buf,READ_BUFFER);
			if(( (fherr = tofile->Write(buf, writesize)) != writesize))
			{
				delete fromfile;
				delete tofile;
				return(B_ERROR);			
			}
			remainsize -= writesize;
		}
		delete tofile;
	}
	return true;
}
//
//		MSDInterface: delete picture
bool MSDInterface::deleteItem(int index)
{
	return removeMSDItem(index);
}
//
//		MSDInterface: get Name
char* MSDInterface::getName(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return (char *)item->GetName();
}
//
//		MSDInterface: get Size
int MSDInterface::getSize(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetSize();
}
//
//		MSDInterface: get X Res
int MSDInterface::getXRes(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetXRes();
}
//
//		MSDInterface: get Y Res
int MSDInterface::getYRes(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return item->GetYRes();
}
//
//		MSDInterface: get Date
char* MSDInterface::getDate(int index)
{
	MSD_Item *item;
	item = getMSDItem(index);
	return (char *)item->GetDate();
}
//
//		MSDInterface: get Thumbnail
BBitmap* MSDInterface::getThumb(int index) 
{
	BBitmap *source;
	BBitmap *thumb;
	//	Get the image of the scsi device
	MSD_Item *item;
	item = getMSDItem(index);
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,"This is the item file: %s\n",item->GetItemPath());
		fclose(file);
	#endif
	source = BTranslationUtils::GetBitmapFile(item->GetItemPath());
	BRect rect(0,0,150,113);
	thumb = new BBitmap(rect,B_RGB32);
	//	Rescale the bitmap
	//RescaleBitmap((char *)item->GetItemPath(),thumb);
	return thumb;
}
//
//		MSDInterface: get Device Name
char*	MSDInterface::getDeviceName()
{
	return msdDeviceName;
}
//
//		MSDInterface: get Version
char*	MSDInterface::getVersion()
{
	return msdVersion;
}
//
//
void MSDInterface::getMSDItems(const char *path)
{
	// TODO :: 	Surfing in the file structure of the mass storage device
	//			and create a MSD_Item when we find a picture.
	BFile file;
	BNodeInfo fileInfo;
	char type[256];
	status_t status;
	BDirectory dir(path);
	MSD_Item *localItem;
  	if(dir.InitCheck() == B_OK)
  	{
    	BEntry entry;
    	while(dir.GetNextEntry(&entry,true) >= 0) 
    	{
      		BPath name;
      		entry.GetPath(&name);
      		if(entry.IsDirectory()) 
        		getMSDItems(name.Path());
      		else if(entry.IsFile())
      		{
        		status = file.SetTo(name.Path(),B_READ_ONLY);
        		if(status == B_OK)
        		{
        			fileInfo.SetTo(&file);
        			if(fileInfo.GetType(type) == B_OK)
        			{
        				BString tmp;
        				tmp.Insert(type,256,0);
        				if(tmp.FindFirst("image/jpeg") > B_ERROR)
        				{
        					localItem = new MSD_Item();
							// set the handle
							localItem->setHandle(numberOfItems);
							// set the path
							localItem->setItemPath(name.Path());
							off_t size;
							file.GetSize(&size);
							localItem->setSize((uint32)size);
							// set the name
							localItem->setName(name.Leaf());
							// set the resolution
							BBitmap *image;
							BTranslatorRoster *proster = BTranslatorRoster::Default();
							if (!proster)
							{
								printf("MSD - TranslationRoster problem\n");
								return;
							}
							translator_info info;
							memset(&info, 0, sizeof(translator_info));
							BMessage ioExtension;
							int ret = proster->Identify(&file, &ioExtension, &info, 0, NULL,B_TRANSLATOR_BITMAP);
							if(ret == B_NO_TRANSLATOR)
							{
								printf("MSD - No Translator found\n");
								return;
							}
							else if(ret == B_NOT_INITIALIZED)
							{
								printf("MSD - Not Initialized found\n");
								return;	
							}
							image = BTranslationUtils::GetBitmapFile(name.Path(),proster);
							BRect bounds;
							bounds = image->Bounds();
							localItem->setRes(bounds.Width(),bounds.Height());
							delete(image);
							// set the capture date
							time_t captureDate;
							file.GetCreationTime(&captureDate);
							struct tm *ptr;
							char *tmpdate;
							tmpdate = new char[10];
							ptr = gmtime(&captureDate);
							strftime(tmpdate,100,"%d/%m/%Y",ptr);
							localItem->setDate(tmpdate);
							// Insert the Item in the list
							MSDItems.insert(pair<int,MSD_Item*>(numberOfItems,localItem));
							numberOfItems++;
        				}
        			}
        		}
      		}
      	}
    }
}
//
//		MSDInterface: camera Connection
bool MSDInterface::cameraConnected() 
{
	return camConnected;
}
//
//		MSDInterface : IsMounted()
bool
MSDInterface::IsMounted() const
{
        return _PartitionData()->mounted;
}
//
// 		MSDInterface : GetMountPoint
status_t
MSDInterface::GetMountPoint(BPath* mountPoint) const
{
        /*if (!mountPoint || !ContainsFileSystem())
                return B_BAD_VALUE;

        // if the partition is mounted, return the actual mount point
        BVolume volume;
        if (GetVolume(&volume) == B_OK) {
                BDirectory dir;
                status_t error = volume.GetRootDirectory(&dir);
                if (error == B_OK)
                        error = mountPoint->SetTo(&dir, NULL);
                return error;
        }

        // partition not mounted
        // get the volume name
        const char* volumeName = ContentName();
        if (!volumeName || strlen(volumeName) == 0)
                volumeName = Name();
        if (!volumeName || strlen(volumeName) == 0)
                volumeName = "unnamed volume";

        // construct a path name from the volume name
        // replace '/'s and prepend a '/'
        BString mountPointPath(volumeName);
        mountPointPath.ReplaceAll('/', '-');
        mountPointPath.Insert("/", 0);

        // make the name unique
        BString basePath(mountPointPath);
        int counter = 1;
        while (true) {
                BEntry entry;
                status_t error = entry.SetTo(mountPointPath.String());
                if (error != B_OK)
                        return error;

                if (!entry.Exists())
                        break;
                mountPointPath = basePath;
                mountPointPath << counter;
                counter++;
        }

        return mountPoint->SetTo(mountPointPath.String());*/ return B_OK;
}
//
// 	MSDInterface : Mount
dev_t
MSDInterface::Mount(const char* mountPoint, uint32 mountFlags,const char* parameters)
{
        /*if (IsMounted() || !ContainsFileSystem())
                return B_BAD_VALUE;*/

        // get the partition path
        BPath partitionPath;
        status_t error;// = GetPath(&partitionPath);
        //if (error != B_OK)
        //        return error;

        // create a mount point, if none is given
        bool deleteMountPoint = false;
        BPath mountPointPath;
        /*if (!mountPoint) {
                // get a unique mount point
                error = GetMountPoint(&mountPointPath);
                if (error != B_OK)
                        return error;

                mountPoint = mountPointPath.Path();

                // create the directory
                if (mkdir(mountPoint, S_IRWXU | S_IRWXG | S_IRWXO) < 0)
                        return errno;

                deleteMountPoint = true;
        }*/

        // mount the partition
        dev_t device = fs_mount_volume(mountPoint, partitionPath.Path(), NULL,
                mountFlags, parameters);

        // delete the mount point on error, if we created it
        if (device < B_OK && deleteMountPoint)
                rmdir(mountPoint);

        // update object, if successful
        //if (device >= 0)
        //        error = Device()->Update();

        return device;
}
//
// 	MSDInterface : Unmount
status_t
MSDInterface::Unmount(uint32 unmountFlags)
{
        //if (!IsMounted())
        //       return B_BAD_VALUE;

        // get the partition path
        BPath path;
        status_t status; // = GetMountPoint(&path);
        if (status != B_OK)
                return status;

        // unmount
        status = fs_unmount_volume(path.Path(), unmountFlags);

        // update object, if successful
        //if (status == B_OK)
        //        status = Device()->Update();

        return status;
}
//
//		MSDInterface: get MSD Item
MSD_Item* MSDInterface::getMSDItem(int handle)
{
	map<uint32,MSD_Item*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == handle)
		{
			#ifdef DEBUG
				FILE	*file;
				file = fopen(LOGFILE,"a");
				fprintf(file,"Get MSItem\n");
				fclose(file);
			#endif
			// return the MSD Item
			return (*i).second;
		}
		i++;
	}
	return NULL;
}
//
//		MSDInterface: get MSD Item
bool MSDInterface::removeMSDItem(int handle)
{
	map<uint32,MSD_Item*>::iterator i = MSDItems.begin();
	while(i != MSDItems.end())
	{
		if((*i).first == handle)
		{
			//
			printf("MSD - Delete Picture\n");
			//TODO:: remove the item
			BEntry entry((*i).second->GetItemPath());
			entry.Remove();
			(*i).second->setHandle(-1);
		}
		i++;
	}
	return true;
}
//
//	MSDInterface:RescaleBitmap
void MSDInterface::RescaleBitmap(char *srcfile, BBitmap *dest)
{
	#ifdef DEBUG
		FILE	*file;
		file = fopen(LOGFILE,"a");
		fprintf(file,"MS - RescaleBitmap :: Begin rescaling \n");
		fclose(file);
	#endif
	BRect rect(0,0,150,113);
	dest = new BBitmap(rect,B_RGB32,true);
	BitmapView *bmview = new BitmapView(rect);
	bmview->LoadBitmap(srcfile);
	dest->AddChild(bmview);
	dest->Lock();
	bmview->Invalidate();
	bmview->Sync();
	bmview->RemoveSelf();
	dest->Unlock();
	delete(bmview);
	#ifdef DEBUG
		file = fopen(LOGFILE,"a");
		fprintf(file,"MS - RescaleBitmap :: End rescaling \n");
		fclose(file);
	#endif
}
// 
//		MSDInterface: MSD_logError
int MSDInterface::MSD_logError(int ErrorMes)
{
	char 				*errorMessage;
	
	switch(ErrorMes)
	{
		case MS_OPEN_CAMERA:
			errorMessage = "MS: Could not mount the scsi device\n";
			break;
		case MS_CLOSE_CAMERA:
			errorMessage = "MS: Could not unmount the scsi device\n";
			break;
		default:
			errorMessage = "MS: An unexpected error occured\n";
	}
	// write the errorMessage into the logfile
	#ifdef DEBUG
	FILE	*file;
	file = fopen(LOGFILE,"a");
	fprintf(file,errorMessage);
	fclose(file);
	#endif
	return(ErrorMes);
}
//
//		MSDInterface :: MSD_LogValue
void MSDInterface::MSD_logValue(int ValueMes, int Value)
{
	char 				*valueMessage;
	
	switch(ValueMes)
	{
		default:
			valueMessage = "MS: An unexpected error occured\n";
	}
	// write the errorMessage into the logfile
	#ifdef DEBUG
	FILE	*file;
	file = fopen(LOGFILE,"a");
	fprintf(file,valueMessage,Value);
	fclose(file);
	#endif
}
