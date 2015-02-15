#include <SerialPort.h> 		// For Serial Comms
#include <Errors.h> 			// For Return Type
#include <AppFileInfo.h> 		// For Version Info
#include <Bitmap.h>
#include <TranslationUtils.h>
#include <String.h>
#include <cstdio>
#include <Path.h>
#include <string>
#include <vector>
#include <map>
#include <Looper.h>
#include <iostream.h>
#include "preferences.h"
#include <SerialPort.h>

//extern BSerialPort *ComPort;

#include "packet.h"


class MSAC_SR1
{
	public:
		MSAC_SR1::MSAC_SR1(BString &serialsettings, data_rate &serialspeed);
		MSAC_SR1::~MSAC_SR1();

		status_t init();
		status_t openCamera(void);
		status_t closeCamera(void);

		int getNumberOfPics();

		status_t setCurrentPicture(int picturenumber);
		status_t downloadPicture(BPath savedir);

		void getSupportedCameras(vector<string> &);

		status_t deletePicture();
		status_t takePicture();
		
		status_t getImageName(char* &name);
		status_t getImageSize(int &size);
		status_t getImageDate(char* &date);
		
		BWindow * configurePlugin(BPoint middle);
		
		status_t getThumbnail(BBitmap* &);
		
		status_t getLevel3FunctionNames(map<char*, char*> &menunames);
		
		status_t setMessageTarget(BLooper* mtarget);
		
		BWindow* ConfigureCamera(BPoint middle);

	protected:
		status_t 	setSpeed(data_rate speed);

		status_t 	MakePacket(Packet *p, const unsigned char *buffer, unsigned short int length);

		unsigned char CalcCheckSum(Packet *p);
		

		status_t 	ReadPacket(Packet *p);
		status_t 	WritePacket(Packet *p);
		status_t 	Converse(Packet *p, const unsigned char *buffer, int length);
		status_t MSAC_SR1::CheckPacket(Packet *p);
	
	private:
		BSerialPort *ComPort;
		BString		portsettings;
		data_rate	portspeed;		// We initialize at 9600bps but do all real work at this speed
		int			picture_count;
		int 		current_picture;
		int			sequence_id;

		status_t ReadCommsPacket(Packet *dst);
		Packet* ReadPacket();
		int ReadCommByte(unsigned char *byte);
		int Read(unsigned char *buffer, int *length);
};