/* ExifParse.h */

#ifndef EXIFPARSE_H
#define EXIFPARSE_H

#define JPG_SOI 0xffd8
#define JPG_EOI 0xffd9
#define JPG_APP1 0xffe1


#define ExifState_init 0
#define ExifState_exif_init 1
#define ExifState_exif_header 2
#define	ExifState_tiff_header 3
#define	ExifState_ifdnum 4
#define	ExifState_ifdentry 5
#define ExifState_ifdoffset 6
#define ExifState_thumbdata 7

#define ifdf_ubyte 1
#define ifdf_string 2
#define ifdf_ushort 3
#define ifdf_ulong 4
#define ifdf_urat 5
#define ifdf_sbyte 6
#define ifdf_undef 7
#define ifdf_sshort 8
#define ifdf_slong 9
#define ifdf_srat 10
#define ifdf_float 11
#define ifdf_dfloat 12


struct ifd_entry
{
struct ifd_entry	*prev;
struct ifd_entry	*next;
unsigned short		tagnum;
unsigned short		format;
unsigned long		size;
unsigned long		offset;
unsigned char		data[1];
};

int					exifparse(const class BUSBEndpoint *iept, class BMallocIO *memfh, uint32 length, struct PictureDesc *item);
void 				SetJPEGFileAttrs(class BFile *fh, PictureDesc *item);

static const char Exif_header[] = {'E','x','i','f', 0, 0};

#endif
