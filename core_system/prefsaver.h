/*
****************************************************************
* Copyright (c) 2004-2005,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//		File defenition

#ifndef PREF_SAVER_H
#define PREF_SAVER_H

//
//		Includes
#include <FindDirectory.h>
#include <Path.h>
#include <File.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Entry.h>

//
//		Preference Saver Class
class BeCam_PrefSaver
{
	public:
		BeCam_PrefSaver(const char *name,const int no_of_fields);
		~BeCam_PrefSaver();
		
		void AddField(const char *f,const char *entry);
		
		bool GetString(const char *f,char *entry,const unsigned short buf_size);
		
		bool SaveFile();
		bool GetLine(BEntry *the_file,long line_number,char *buffer); 
			
	private:		
		char 	**field ;
		char 	**values;
	
		int Get_Position(const char *f);
		
	protected: 
		int max_no_of_lines;
		int current_number;
		BEntry *file;
};

#endif

		
		
