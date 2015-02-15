#ifndef _PREF__SAVER_H_
#define _PREF__SAVER_H_





#include <FindDirectory.h>
#include <Path.h>
#include <File.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

class Pref_Saver
{
	public:
		Pref_Saver(const char *name,const int no_of_fields);
		~Pref_Saver();
		
		void AddField(const char *f,const bool entry); 
		void AddField(const char *f,const long entry); 
		void AddField(const char *f,const char *entry);
		void AddField(const char *f,const double *entry);
		
		bool GetBool(const char *f,bool *entry); //all return false if successful. 
		bool GetInt(const char *f,long *entry); 
		bool GetString(const char *f,char *entry,const unsigned short buf_size);
		bool GetFloat(const char *f,double *entry);
		
		bool SaveFile();
		bool GetLine(BEntry *the_file,long line_number,char *buffer); 
			//useful spin-off function that grabs a line of a file (by counting '\n' s)
			//Its only to be used for small files though, ie: less than 1mb.
			//It'll get really slow otherwise.
			//GetLine() returns false if sucessful.
			
	private:		
		char 	**field ;//Arrays containing of lines in text form
		char 	**values;//each one is of this format:
						 // "field = value\n"
						 //The field is user-defined (can be any string)
						 //value is a string, which might be a 'string-isted' number
	
		int Get_Position(const char *f); //checks for existing Field
		
	protected: 
		int max_no_of_lines; //used to keep check
		int current_number; //stores the number of fields already set	
		BEntry *file;	//This is the magic file that saves all the stuff
};

#endif

		
		
