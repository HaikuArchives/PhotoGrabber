typedef struct _tagPacket
{
	int valid;
	int length;
	unsigned char buffer[16384];
	unsigned char checksum;
} Packet;

#define DSC_INVALID_CHECKSUM	0x40
#define DSC_INVALID_SEQUENCE	0x41
#define DSC_RESET_SEQUENCE	0x42
#define DSC_RESEND_PACKET	0x43

#define DSC_ESCAPE_CHAR		0x7d
#define DSC_START_CHAR		0xc0
#define DSC_END_CHAR		0xc1

