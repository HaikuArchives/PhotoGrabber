#include "debug.h"
#include <Path.h>
#include <FindDirectory.h>
#include <fstream.h>
#include <cstdio>
#include <iostream.h>
#include <String.h>
#include <Looper.h>

const int32 MSG_PLUGINDEBUG			= 0x01012000;

extern BLooper* msgtarget;

void debug(char* debugstring)
{
	sendmessage(debugstring);
}

void debug(BString debugstring)
{
	sendmessage((char *)debugstring.String());
}

void debug(BString debugstring1, BString debugstring2)
{
	char* stringtosend = new char[debugstring1.Length() + debugstring2.Length() + 3];
	sprintf(stringtosend, "%s %s",debugstring1.String(),debugstring2.String());
	sendmessage(stringtosend);
}

void debug(BString debugstring1, int debugval2)
{
	char* stringtosend = new char[debugstring1.Length() + 30];
	sprintf(stringtosend, "%s %d",debugstring1.String(),debugval2);
	sendmessage(stringtosend);
}

void sendmessage(char* message)
{
	if (msgtarget != NULL)
	{
		BMessage *msg;
		msg = new BMessage(MSG_PLUGINDEBUG);
		msg->AddString("debugmsg", message);
		msg->AddString("plugname", "libbdcp_msac-sr1");
		msgtarget->PostMessage(msg);
		delete msg;
	}
}
