#include "preferences.h"
#include "prefsaver.h"

void LoadSettingsFromFile(void)
{
	Pref_Saver prefs("bdcp.kodakdc210.prefs",10); 
	speed = B_115200_BPS;
	long *tempint;
	tempint = new long;
	char tempbuf[255];
	if (prefs.GetString("serialport", tempbuf, 255))
		strcpy(tempbuf, "serial1");
	serialportsetting.SetTo(tempbuf);	
	long *tempspeed;
	tempspeed=new long; 
	if (prefs.GetInt("speed",tempspeed))
		*tempspeed=4;
	if (*tempspeed == 0) speed = B_9600_BPS;
	else if (*tempspeed == 1) speed = B_19200_BPS;
	else if (*tempspeed == 2) speed = B_38400_BPS;
	else if (*tempspeed == 3) speed = B_57600_BPS;
	else if (*tempspeed == 4) speed = B_115200_BPS;
}

void SaveSettingsToFile(void)
{
	Pref_Saver prefs("bdcp.kodakdc210.prefs",10); 
	prefs.AddField("serialport", (char *)serialportsetting.String());
	if (speed == B_9600_BPS) prefs.AddField("speed", (long)0);
	else if (speed == B_19200_BPS) prefs.AddField("speed", (long)1);
	else if (speed == B_38400_BPS) prefs.AddField("speed", (long)2);
	else if (speed == B_57600_BPS) prefs.AddField("speed", (long)3);
	else if (speed == B_115200_BPS) prefs.AddField("speed", (long)4);
	prefs.SaveFile(); 
}
