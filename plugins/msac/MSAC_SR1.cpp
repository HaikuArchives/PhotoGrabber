#include "debug.h"
#include "MSAC_SR1.h"

/*
	This code is borrows heavily from the sony library code included within the gphoto distribution. 
	That code was written by Juergen Weigert <jw@netvision.de>.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    http://www.gnu.org/copyleft/gpl.html
*/
static const unsigned char dsc_sequence[] 			= {14,0,32,34,66,68,100,102,134,136,168,170,202,204,236,238,255};
static const unsigned char IdentString[] 			= {0,1,1,'S','O','N','Y',' ',' ',' ',' ',' '};
static const unsigned char EmptyPacket[]			= {0}; /* null packet */
static unsigned char SetTransferRate[]				= {0,1,3,0}; // Not const because SetSpeed changes this
static unsigned char SendImage[]					= {0,2,'1',0,1,0,0};
static const unsigned char SendNextImagePacket[]	= {0,2,'1',0};
static const unsigned char SendThumbnail[]			= {0,2,'0',0};
static unsigned char SelectImage[]					= {0,2,48,0,0,0,0};
static const unsigned char StillImage[] 			= {0,2,2,0,14,'/','D','C','I','M','/','1','0','0','M','S','D','C','F'};
static const unsigned char MpegImage[]  			= {0,2,2,0,16,'/','M','S','S','O','N','Y','/','M','O','M','L','0','0','0','1'};
static const unsigned char SendImageCount[]			= {0,2,1};
static const unsigned char START_PACKET				= 192;
static const unsigned char END_PACKET				= 193;

Packet CameraInvalid 			= {0, 2, {131,125}, 93};
Packet ResendPacket 			= {0, 4, {129,2,'1',0}, 'L'};
unsigned char PacketCodes[2] 	= {192,193};
unsigned char ESC_START_STRING[]			= { 0x7d, 0xe0 };
unsigned char ESC_END_STRING[]			= { 0x7d, 0xe1 };
unsigned char ESC_ESC_STRING[]			= { 0x7d, 0x5d };


MSAC_SR1::MSAC_SR1(BString &serialsettings, data_rate &serialspeed) {
	debug("MSAC_SR1 instantiated");
	
	portsettings.SetTo(serialsettings);
	portspeed = serialspeed;
	
	debug("Set portsettings to", portsettings);
	debug("Set portspeed to", portspeed);
}

status_t MSAC_SR1::init() {
	status_t res;
	ComPort = new BSerialPort();
	ComPort->SetDataRate(B_9600_BPS);
	ComPort->SetFlowControl(0);
	ComPort->SetBlocking(true);
	ComPort->SetTimeout(3000000);
	res = ComPort->Open(portsettings.String());
	if (res < B_NO_ERROR) return(B_ERROR);

	int count = 0;
	Packet dp;

	for (count=0; count<3; count++) {
		debug("Attempting to contact reader", count);
		sequence_id = 0;
		if (Converse(&dp, IdentString,12) == B_NO_ERROR) break;
		snooze(2000);
	}
	
	if (count >= 3)	{
		debug("Error opening camera!");
		return(B_ERROR);
	}

	if (setSpeed(speed) != B_NO_ERROR) {
		debug("Leaving openCamera - unable to set speed!");
		return B_ERROR;
	}
	
	debug("Leaving openCamera - no errors");
	return B_NO_ERROR;
}

MSAC_SR1::~MSAC_SR1() {
	debug("closing connection");
	Packet dp; 
	
	while (sequence_id) {
		Converse(&dp, EmptyPacket, 1);
	}
	
	setSpeed(B_9600_BPS);
	ComPort->SetDataRate(B_9600_BPS); 
    ComPort->Close(); 
	debug("closeCamera complete");
}


int MSAC_SR1::getNumberOfPics() {
	Packet dp;

	Converse(&dp, SetTransferRate, 4);

	if (Converse(&dp, StillImage, sizeof(StillImage)) == B_NO_ERROR) {
		if (Converse(&dp, SendImageCount, 3) == B_NO_ERROR) {
			picture_count = (unsigned short int) dp.buffer[5];
			debug("Number of Pictures", picture_count);
			return picture_count;
		}
		else {
			debug("SendImageCount Failed");
		}

	} else {
		debug("Init Image Failed\n");
	}

	return 0;
}

status_t MSAC_SR1::setCurrentPicture(int picturenumber) {
	debug("Setting current picture", picturenumber);
	current_picture = picturenumber;
	return(B_NO_ERROR);
}

status_t MSAC_SR1::downloadPicture(BPath savedir) {
	int picno = current_picture + 1;
	debug("Downloading picture", picno);
	debug("Download Path: ", savedir.Path());
	
	//Obtain the filename 
    char* filename = new char[B_FILE_NAME_LENGTH]; 
    sprintf (filename, "Image %d", picno); 
    savedir.Append(filename); 
    
   	debug("Download Filename: ", savedir.Path());
	//Create the file to save the image to 
    BFile* file = new BFile(savedir.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE); 

	long t = time(0);

	int sc=11; /* count of bytes to skip at start of packet */
	Packet dp;

	SelectImage[4] = picno;
	Converse(&dp, SelectImage, 7);

	if(!Converse(&dp, StillImage, sizeof(StillImage))) debug("store init failed");

	snooze(50000);

	SendImage[4] = picno;
	Converse(&dp, SendImage, 7);

	for(;;)
	{
		//if (verbose > 1) { putchar('.'); fflush(stdout); }
		file->Write((char *)dp.buffer + sc, dp.length - sc);
		//fwrite((char *)dp.buffer+sc, sizeof(char), dp.length-sc, temp);
		sc=7;

		if (3 == dp.buffer[3]) break;

		Converse(&dp, SendNextImagePacket, 4);
		// old sig_cleanup code if (do_exit) break;
	}

	debug("Download time: ", time(0) - t);
	
	debug("Done downloading picture", picno);
	return(B_NO_ERROR);
}

status_t MSAC_SR1::deletePicture() {
	debug("deletePicture called. I don't know how to do that.");
	return(B_ERROR);
}

status_t MSAC_SR1::getThumbnail(BBitmap* & bitmap) {
	debug("getting thumbnail for image", current_picture);

	int sc=11; /* count of bytes to skip at start of packet */
	Packet dp;
	
	sc = 0x247;

	if (Converse(&dp, StillImage, 19) != B_NO_ERROR) {
		debug("StillImage Failed");
		return(B_ERROR);
	}

	SelectImage[4] = current_picture + 1;
	Converse(&dp, SelectImage, 7);

	BMallocIO *jpgStore = new BMallocIO();

	jpgStore->Write("\xff\xd8\xff", 3);
	
	int loops = 0;
	while (loops++ < 1000) {
		Converse(&dp, SendThumbnail, 4);
		jpgStore->Write((char *)dp.buffer+sc, dp.length - sc);
		sc=7;
		if (3 == dp.buffer[4]) break;
	}

	bitmap = BTranslationUtils::GetBitmap(jpgStore);

	debug("Done retrieving thumbnail", current_picture);	
	return B_NO_ERROR;
}

status_t MSAC_SR1::getImageName(char* &name) {
	Packet dp;
	
	SelectImage[4] = current_picture + 1;
	Converse(&dp, SelectImage, 7);

	name = strdup((char*)dp.buffer+5);

	return(B_NO_ERROR);
}

status_t MSAC_SR1::getImageDate(char* &date) {
	return(B_ERROR);
}

status_t MSAC_SR1::getImageSize(int &size) {
	return(B_ERROR);
}

// Utility Functions
status_t MSAC_SR1::setSpeed(data_rate speed) {
	debug("Setting transfer speed to requested rate: ", speed);
	int r;
 	switch (speed){
    	case B_115200_BPS: 	r = 4; break;
    	case B_57600_BPS: 	r = 3; break;
    	case B_38400_BPS: 	r = 2; break;
    	case B_19200_BPS: 	r = 1; break;
	    default:
    	case B_9600_BPS: 	r = 0; break;
    }

	debug("rate id", r);

	SetTransferRate[3] = r;

	Packet sp;

	debug("SetTransferRate");
	Converse(&sp, SetTransferRate, 4);

	debug("SetDataRate");
	ComPort->SetDataRate(speed);

	debug("Sending EmptyPacket");
	Converse(&sp, EmptyPacket, 1);

	snooze(50000);
	return(B_NO_ERROR);
}

status_t MSAC_SR1::MakePacket(Packet *p, const unsigned char *buffer, unsigned short int length) {
	p->length = 0;

	while (length--) p->buffer[p->length++] = *(buffer++);

	if (255 == dsc_sequence[++sequence_id]) 
		sequence_id = 0;

	p->buffer[0] = dsc_sequence[sequence_id++];

	if (255 == dsc_sequence[sequence_id])
		sequence_id = 0;

	p->checksum = CalcCheckSum(p);

	return(B_NO_ERROR);
}
unsigned char MSAC_SR1::CalcCheckSum(Packet *p) {
	unsigned short int o = 0;
	unsigned long int sum = 0;

	while(o < p->length) sum += p->buffer[o++];

	return 256-(sum&255);
}

status_t MSAC_SR1::ReadPacket(Packet *p) {
	return(B_ERROR);
}
status_t MSAC_SR1::WritePacket(Packet *p) {
	unsigned short int count;
	int bytecount;

	bytecount = ComPort->Write((char *)&START_PACKET, 1);

	p->buffer[p->length] = p->checksum;

	for(count=0; count<p->length+1; count++)
	{
		switch((unsigned char)p->buffer[count])
		{
			case DSC_ESCAPE_CHAR:
				ComPort->Write(ESC_ESC_STRING, 2);
				break;

			case DSC_START_CHAR:
				ComPort->Write(ESC_START_STRING, 2);
				break;

			case DSC_END_CHAR:
				ComPort->Write(ESC_END_STRING, 2);
				break;

			default:
				bytecount = ComPort->Write((char *)&p->buffer[count], 1);
				break;
		}
	}

	ComPort->Write((char *)&END_PACKET, 1);
	return(B_NO_ERROR);
}

status_t MSAC_SR1::Converse(Packet *out, const unsigned char *str, int len) {
	Packet ps;
 	int invalid_sequence=0;
	int count;

	MakePacket(&ps, str, len);

	for(count=0; count < 10; count++)
	{
		WritePacket(&ps);

		if (ReadCommsPacket(out) == B_NO_ERROR) {
			switch(CheckPacket(out)) {
				case DSC_INVALID_CHECKSUM:
					if (invalid_sequence) {
						MakePacket(&ps, str, len);
						break;
					}

					debug("Checksum invalid");
					ps.buffer[0] = 129;
					ps.checksum = CalcCheckSum(&ps);
					break;

				case DSC_INVALID_SEQUENCE:
					invalid_sequence = 1;
					MakePacket(&ps, str, len);
					break;

				case DSC_RESET_SEQUENCE:
					sequence_id = 0;
					return(B_NO_ERROR);
		
				case DSC_RESEND_PACKET:
					debug("Resending Packet");
					break;

				case B_NO_ERROR:
					return(B_NO_ERROR);

				default:
					debug("Unknown Error");
					break;
			}
		} else {
			debug("Incomplete packet");
			ps.buffer[0] = 129;
			ps.checksum = CalcCheckSum(&ps);
		}
	}

	debug("Converse: Failed to read packet.");
	return(B_ERROR);
}

status_t MSAC_SR1::CheckPacket(Packet *p)
{
	unsigned char c = CalcCheckSum(p);

	if(c!=p->checksum)
		return DSC_INVALID_CHECKSUM;

	if(129==p->buffer[0])
		return DSC_RESEND_PACKET;

	if(dsc_sequence[sequence_id] != p->buffer[0])
		return DSC_INVALID_SEQUENCE;

	return B_NO_ERROR;
}

status_t MSAC_SR1::ReadCommsPacket(Packet *dst) {
	Packet *p;

	p = ReadPacket();

	if (p) {
		memcpy(dst, p, sizeof(Packet));
		return(B_NO_ERROR);
	}

	return(B_ERROR);
}

/***************************************************************
*
*
*/
int MSAC_SR1::Read(unsigned char *buffer, int *length) {
	int len = ComPort->Read(buffer, *length);

	if(len < 0)
	{
		debug("Read failed");
		*length = 0 ;
	}

	return len;
}


/***************************************************************
*
*
*/
int MSAC_SR1::ReadCommByte(unsigned char *byte) {
  static unsigned char buf[256];
  static int bytes_read = 0;
  static int bytes_returned = 0;

  if (bytes_returned < bytes_read)
    {
      *byte = buf[bytes_returned++];
      return 1;
    }

  if ((bytes_read = ComPort->Read(buf, sizeof(buf))) < 0)
    debug("ReadCommByte failed");

  bytes_returned = 0;
  if (bytes_read) *byte = buf[bytes_returned++];
  return (bytes_read > 0) ? 1 : bytes_read;
}

/*******************************************************************
*
*
*
*/
Packet* MSAC_SR1::ReadPacket() {
	unsigned int n;
	unsigned char byte=0;
	static Packet p;

	p.length = 0;

	for(n=0; n<2; n++)
	{
		for(byte=0;byte!=(unsigned char)PacketCodes[n];)
		{
			if(ReadCommByte(&byte) <=0)
				return (Packet *)0;

			if(n>0)
				if(DSC_ESCAPE_CHAR == byte)
				{
					unsigned char extra;

					ReadCommByte(&extra);

					switch(extra)
					{
						case 1:
						case 7:
						case 0xe1:
						case 0xe0:
							extra &= 0xcf;
							p.buffer[p.length++]=extra;
							continue;
						case 0x5d:
							p.buffer[p.length++]=byte;
							continue;
						default:
							p.buffer[p.length++]=byte;
							p.buffer[p.length++]=extra;
							continue;
					}
				}
				else
					p.buffer[p.length++]=byte;
		}
	}

	p.length-=2;
	p.checksum = p.buffer[p.length];

	return &p;
}
