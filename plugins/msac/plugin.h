#include <SerialPort.h> // For Serial Comms
#include <Errors.h> // For Return Type
#include <AppFileInfo.h> // For Version Info
#include <String.h>
#include <cstdio>
#include <Path.h>
#include <string>
#include <vector>
#include <map>
#include <Looper.h>

#include "MSAC_SR1.h"

const int32 MSG_UPDATEBAR			= 0x01012001;

BLooper* msgtarget;
int currentpicturenumber;
bool haveigotfilenames;

BString serialportsetting;
data_rate speed;
BSerialPort *ComPort;


extern int currentpicturenumber;
extern bool haveigotfilenames;
extern map<int, char*> imagenames;
extern map<int, int> imagesizes;
extern map<int, char*> imagedates;


//Level 1 Functions - MUST be implemented fully to be BDCP2 Compliant

extern "C" int get_BDCP_API_Revision(void);
extern "C" void getPluginVersion(version_info &);
extern "C" status_t openCamera(void);
extern "C" status_t closeCamera(void);
extern "C" status_t getNumberofPics(int &);
extern "C" status_t setCurrentPicture(int picturenumber);
extern "C" status_t downloadPicture(BPath savedir);
extern "C" void getSupportedCameras(vector<string> &);

//Level 2 Functions - Optional
extern "C" status_t deletePicture();
extern "C" status_t takePicture();
extern "C" status_t getImageName(char* &name);
extern "C" status_t getImageSize(int &size);
extern "C" status_t getImageDate(char* &date);
extern "C" BWindow * configurePlugin(BPoint middle);
extern "C" status_t getThumbnail(BBitmap* &);
extern "C" status_t getLevel3FunctionNames(map<char*, char*> &menunames);
extern "C" status_t setMessageTarget(BLooper* mtarget);
status_t getfiledata();


extern BSerialPort *ComPort;

//Level 3 Functions - Optional
extern "C" BWindow* ConfigureCamera(BPoint middle);

MSAC_SR1 *stickReader;