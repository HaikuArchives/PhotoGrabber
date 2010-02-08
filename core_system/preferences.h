/*
****************************************************************
* Copyright (c) 2004-2010,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//		File defenition
#ifndef	PREFERENCES_H
#define PREFERENCES_H
//
// Includes
#include <String.h>
#include <storage/StorageDefs.h>
#include <FindDirectory.h>
#include <Path.h>
#include <stdio.h>
#include <stdlib.h>
//
// Local includes
#include "settings.h"
//	Variables
static char 	**field ;
static char 	**values;
static int max_no_of_lines;
static int current_number;
static BEntry *file;

//
// GetLine
static bool GetLine(BEntry *_file,long line_number,char *buffer)
{
	char *buff;
 	buff=new char[1];
	long i=0,num=0;
	char buf=0;
 	off_t size;
 	BFile the_file(_file,B_READ_ONLY);
 	if (the_file.InitCheck()!=B_OK)
 		return true;
 	the_file.GetSize(&size);
 	the_file.Seek(0,SEEK_SET); //im paranoid
 	while(i<size)
	{
		if (buf==10) //newline
			num++;
		if (num>=line_number) 
		{//grab next line
			buf=0;
			int pos=0;
			while ((i<size)&&(buf!=10))
			{
				the_file.Read(buff,1);
				buf=*buff;
				if (buf!=10)
					buffer[pos++]=buf;
				i++;
			}
			buffer[pos]=0;
			delete buff;
			return true;		
		}
		the_file.Read(buff,1);
		buf=*buff; //a hack because Read((void*)buf,1) doesnt actually seem to do anything
		i++;
	}
	delete buff;//if it gets this far, we need to delete it.
	return true;
}
//
// Get Fields
static void GetFields (const char *name,const int no_of_fields)
{
	BPath prefsPath;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &prefsPath)!=B_OK)
    	return; 
	if (prefsPath.Append(name)!=B_OK)
		return;
 	field=new char*[no_of_fields];
 	values=new char*[no_of_fields];
 	max_no_of_lines=no_of_fields;
 	file=new BEntry(prefsPath.Path());
 	if (file->Exists())
 	{
 		char buffer[128];
 		int no_of_lines;
 		GetLine(file,0,buffer);
 		
 		no_of_lines=atoi(buffer);
 		current_number=no_of_lines;
 		int i;
 		short len;
 		for(i=0;i<no_of_lines;i++)
 		{
 			field[i]=new char[64];
 			values[i]=new char[64];
 			
 			
 			GetLine(file,i+1,buffer);	
 			short j=0,k;
 			while(buffer[j++]!='=');
 			if (buffer[j-2]==32)
 				len=2;
 			else
 				len=1;
 			strncpy(field[i],buffer,j-len);
 			field[i][j-len]=0;
 			if (buffer[j]==32)			
 				k=j++;		
 			else
 				k=j;
 			while(buffer[k++]!=0);
 			strncpy(values[i],buffer+j,k-j);
 			values[i][k-j]=0;
 		}
 	}
 	else
 	 	current_number=0;
}
//
// Get position
static int Get_Position(const char *f)
{
	for (int i=0;i<current_number;i++)
	{
		if(strcmp(f,field[i])==0)
			return i;
	}
	if (current_number==max_no_of_lines)
		return -1; //user has been naughty and tried to set more than he'd allowed.
	field[current_number]=new char[64];
 	values[current_number]=new char[64];		
 	current_number++;
	return (current_number-1);
}
//
//	Add a field
static void AddField(const char *f,const char *entry)
{
	int index=Get_Position(f);
	if (index!=-1)
	{
 		strcpy(field[index],f);
		strcpy(values[index],entry);
	}
}
//
// Get a string
static bool GetString(const char *f,char *entry,const unsigned short buf_size)
{
	for (int i=0;i<current_number;i++)
	{
		if (strcmp(f,field[i])==0)
		{
			if (strlen(values[i])+1>buf_size)
				strncpy(entry,values[i],buf_size);
			else
				strcpy(entry,values[i]);//do i need to do this? Don't know completly how strncpy works
			return true;	
		}
		else
			strcpy(entry,"fault");
	}
	return false;
}
//
// Save the file
static bool SaveFile(void)
{
	BFile prefs(file, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (prefs.InitCheck()!=B_OK)
		return true;
	prefs.Seek(0,SEEK_SET); //im paranoid
	
	short pos=0;
	char temp[70];
	sprintf(temp,"%d",(current_number));
	prefs.Write(temp,strlen(temp)); //dont include null termination.
	prefs.Write(" line(s):\n",10);
	
	for (int i=0;i<current_number;i++)
	{
		temp[0]=0;
		strcpy(temp,field[i]);
		strcat(temp," = ");
		strcat(temp,values[i]);
		pos=strlen(temp);
		temp[pos]=10;
		prefs.Write(temp,pos+1);
	}
	return false;
}
//
//		Load the settings from the file
static SETTINGS LoadSettingsFromFile(void)
{
	//BeCam_PrefSaver prefs("photograbber.prefs",4);	
	GetFields("photograbber.prefs",4);
	SETTINGS pgsettings;
    char tempbuf[10];
	//	Default download path
	char tempbufName[B_FILE_NAME_LENGTH];
	if (GetString("defaultdownpath", tempbufName, B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.defaultDownloadPath,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load default download path setting: %s\n",tempbuf);
		strcpy(pgsettings.defaultDownloadPath,"/boot/home/Pictures/PhotoGrabber");
	}
	
	//	Get the plugin name
	if (GetString("pluginname", tempbufName,B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.pluginName,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load plugin name: %s\n",tempbufName);
		strcpy(pgsettings.pluginName,"error");
	}
	
	//	Get the camera name
	if (GetString("cameraname", tempbufName,B_FILE_NAME_LENGTH))
	{
		strncpy(pgsettings.deviceName,tempbufName,B_FILE_NAME_LENGTH);
	}
	else
	{
		printf("PREF - problem load camera name: %s\n",tempbufName);
		strcpy(pgsettings.deviceName,"error");
	}
	
	// Get the Debugging to file parameter
	if(GetString("file", tempbuf, 10))
	{
		if(!strcmp(tempbuf,"true"))
			pgsettings.debugFile = true;
		else
			pgsettings.debugFile = false;
	}
	else
	{
		printf("PREF - problem loading debug file setting: %s\n",tempbuf);
		pgsettings.debugFile = false;
	}
	return pgsettings;
};
//
// Save the settings to the file
static void SaveSettingsToFile(SETTINGS pgsettings)
{
	//BeCam_PrefSaver prefs("photograbber.prefs",4); 
	GetFields("photograbber.prefs",4);
	if(pgsettings.debugFile)
		AddField("file","true");
	else
		AddField("file","false");
	// save the default download path
	AddField("defaultdownpath",pgsettings.defaultDownloadPath);
	//	save the plugin name for bdcp cameras
    AddField("pluginname",pgsettings.pluginName);
	//	save the camera name for bdcp cameras
	AddField("cameraname",pgsettings.deviceName);
	//	
	SaveFile(); 
};
#endif
