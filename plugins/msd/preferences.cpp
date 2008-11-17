/*
****************************************************************
* Copyright (c) 2004-2007,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
#include "preferences.h"
#include "prefsaver.h"
 
void LoadSettingsFromFile(void)
{
	/*Pref_Saver prefs("bdcp.ptp.prefs",10);	
	char tempbuf[10];
	if (prefs.GetString("eptype", tempbuf, 10))
	{
		//printf("PTP - %s\n", tempbuf);
		if(!strcmp(tempbuf,"type2"))
			globalEndpoint = 2;
		else
			globalEndpoint = 1;
	}
	else
	{
		printf("PTP - problem load setting: %s\n",tempbuf);
		globalEndpoint = 1;
	}*/
}

void SaveSettingsToFile(void)
{
	/*Pref_Saver prefs("bdcp.ptp.prefs",10); 
	if(globalEndpoint ==1)prefs.AddField("eptype","type1");
	else prefs.AddField("eptype","type2");
	prefs.SaveFile();*/ 
}
