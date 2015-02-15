#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <String.h>

void sendmessage(char* message);
void debug(char* debugstring);
void debug(BString debugstring);
void debug(BString debugstring1, BString debugstring2);
void debug(BString debugstring1, int debugval2);
#endif
