/*****************************************************************
 * Copyright (c) 2004-2007,	Jan-Rixt Van Hoye					 *
 * All rights reserved.											 *
 * Distributed under the terms of the MIT License.               *
 *****************************************************************/
 
#include "prefsaver.h"
#include <Entry.h>
 
Pref_Saver::Pref_Saver(const char *name,const int no_of_fields)
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
 		GetLine(file,0,buffer); //Get first line which contains the max number of lines (not incl. itself)
 		
 		no_of_lines=atoi(buffer);
 		current_number=no_of_lines;
 		int i;
 		short len;
 		for(i=0;i<no_of_lines;i++)
 		{
 			field[i]=new char[64];
 			values[i]=new char[64];		//SOMETHING WRONG IN HERE!!!
 			
 			
 			GetLine(file,i+1,buffer);	
 			short j=0,k;
 			while(buffer[j++]!='=');		//parse to '='
 			if (buffer[j-2]==32)
 				len=2;
 			else
 				len=1;
 			strncpy(field[i],buffer,j-len); //copy it to the array
 			field[i][j-len]=0;				//null-terminate the string
 			if (buffer[j]==32)			
 				k=j++;		
 			else
 				k=j;
 			while(buffer[k++]!=0);			//parse to end (remember, buffer is null-terminated)
 			strncpy(values[i],buffer+j,k-j);//copy
 			values[i][k-j]=0; 			  //null-terminate.
 		}
 	} //now everthing's in the string arrays
 	else
 	 	current_number=0;
}

bool Pref_Saver::GetLine(BEntry *_file,long line_number,char *buffer) //buffer must be 256 in size
{//this function skips down the number of lines specified by line_number,
 //and returns that line as a string in buffer. 
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

int Pref_Saver::Get_Position(const char *f)
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

void Pref_Saver::AddField(const char *f,const char *entry)
{
	int index=Get_Position(f);
	if (index!=-1)
	{
 		strcpy(field[index],f);
		strcpy(values[index],entry);
	}
}

bool Pref_Saver::GetString(const char *f,char *entry,const unsigned short buf_size)
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

bool Pref_Saver::SaveFile(void)
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

Pref_Saver::~Pref_Saver()
{//delete arrays
	for(int i=0;i<current_number;i++)
	{
		delete values[i];
		delete field[i];
	}
	delete values;
	delete field;

}

		  		
