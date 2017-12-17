/* exifparse.cpp */

//			Includes
#include <stdio.h>
#include <USBKit.h>
#include <File.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Bitmap.h>
#include <View.h>
#include <ByteOrder.h>
#include "exiftags.h"
#include "exifparse.h"
#include "PictureList.h"
#include "bdcpPTP.h"

#define uint16 unsigned short
#define uint32 unsigned long



//		Global data
unsigned char sizeary[13] = {0, 1, 1, 2, 4, 8, 1, 1, 2, 4, 8, 4, 8};
struct ifd_entry *first_entry = NULL, *ready_entry = NULL;

int exifparse(const BUSBEndpoint *iept, BMallocIO *memfh, uint32 length, PictureDesc *item) {
    int bytesread, cnt;

    if (length) {
        uint32 offset = 0, skip = 0, fill = 0, fillcnt = 0;
        uint32 ifd_numcomp = 0, ifd_data = 0, marker = 0, markercnt = 0;
        uint32 nextifd = 0, thumbifd = 0, thumbdata = 0, thumbsize = 0, makernote = 0, interoper = 0;
        uint16 ifd_numdir = 0, ifd_tag = 0, ifd_format = 0;
        char state = 0;
        const char *tmps;
        exiftag *currenttags = exif_tags;
        char* buf = new char[DL_SIZE];
        status_t status;

        while (length) {
            bytesread = length > DL_SIZE ? DL_SIZE : length;
            status = iept->BulkTransfer(buf, bytesread);

            if (status != -1) {
                DEBUG("exifparse: Read %i bytes of thumbnail\n", bytesread);

                for (cnt = 0; cnt < bytesread; cnt++, offset++) {
                    marker = (marker << 8) + (unsigned char) (buf[cnt]);
                    markercnt++;

                    if (skip) {
                        if (!(--skip)) {
                            markercnt = 0;
                        }
                        if (first_entry) {
                            if (first_entry->offset == offset) {
                                fill = (first_entry->size) * sizeary[first_entry->format < 13 ? first_entry->format : ifdf_undef];
                                fillcnt = 0;
                                DEBUG("exifparse: %08lx %08x fill==%ld\n", offset, first_entry->format, fill);
                            }
                        }
                        if (fill) {
                            ready_entry->data[fillcnt++] = buf[cnt];
                            fill--;
                        }
                    } else {
                        switch (state) {
                            case ExifState_init:
                                if (markercnt == 2) {
                                    if (marker == JPG_SOI) {
                                        marker = markercnt = 0;
                                        state = ExifState_exif_init;
                                        DEBUG("exifparse: SOI\n");
                                    } else {
                                        MSG("exifparse: Expected marker not found (%04lx)\n", marker);
                                        return (0);
                                    }
                                }
                                break;

                            case ExifState_exif_init:
                                if (markercnt == 2) {
                                    if (marker == JPG_APP1) {
                                        marker = markercnt = 0;
                                        state = ExifState_exif_header;
                                        skip = 2;
                                        DEBUG("exifparse: APP1\n");
                                    } else {
                                        MSG("exifparse: Unknown init tag %04lx\n", marker);
                                    }
                                }
                                if (markercnt == 4) {
                                    skip = B_BENDIAN_TO_HOST_INT32(marker) - 2; // BENDIAN?
                                    DEBUG("exifparse: Skipping %08lx bytes\n", skip);
                                    marker = markercnt = 0;
                                    state = 0;
                                }
                                break;

                            case ExifState_exif_header:
                                if (buf[cnt] != Exif_header[markercnt - 1]) {
                                    MSG("exifparse: exif no match '%c' != '%c' (%ld)\n", buf[cnt], Exif_header[markercnt - 1], markercnt - 1);
                                    return (0);
                                }
                                if (markercnt == 6) {
                                    DEBUG("exifparse: Exif header OK\n");
                                    state = ExifState_tiff_header;
                                    marker = markercnt = 0;
                                }
                                break;

                            case ExifState_tiff_header:
                                offset = 0;
                                skip = 7;
                                marker = markercnt = 0;
                                ifd_numdir = 0;
                                state = ExifState_ifdnum;
                                break;

                            case ExifState_ifdnum:
                                if (markercnt == 2) {
                                    ifd_numdir = B_BENDIAN_TO_HOST_INT16(marker);
                                    DEBUG("exifparse: numdir %08x marker %08lx\n", ifd_numdir, marker);
                                    marker = markercnt = 0;
                                    state = ExifState_ifdentry;
                                }
                                break;

                            case ExifState_ifdentry:
                                switch (markercnt) {
                                    case 2:
                                        ifd_tag = B_BENDIAN_TO_HOST_INT16(marker);
                                        if (!(tmps = exiftagname(currenttags, ifd_tag))) {
                                            DEBUG("exifparse: Unknown tag: %x\n", ifd_tag);
                                        }
                                        DEBUG("exifparse: tag %04x: %s\n", ifd_tag, tmps ? tmps : "unknown tag");
                                        marker = 0;
                                        break;

                                    case 4:
                                        ifd_format = B_BENDIAN_TO_HOST_INT32(marker);
                                        tmps = exiftagname(&exif_formats[0], ifd_format);
                                        DEBUG("exifparse:           %s\n", tmps ? tmps : "unknown format");
                                        marker = 0;
                                        break;

                                    case 8:
                                        ifd_numcomp = B_BENDIAN_TO_HOST_INT16(marker);
                                        DEBUG("exifparse:           size: %ld\n", ifd_numcomp);
                                        marker = 0;
                                        break;

                                    case 12:
                                        ifd_data = B_BENDIAN_TO_HOST_INT32(marker);
                                        DEBUG("exifparse:           value: %08lx\n", ifd_data);
                                        if (ifd_format == 4 && ifd_numcomp == 1) {
                                            switch (ifd_tag) {
                                                case 0x8769: // ExifOffset
                                                    nextifd = ifd_data;
                                                    break;
                                                case 0x201: // JpegIFOffset
                                                    thumbdata = ifd_data;
                                                    break;
                                                case 0x202: // JpegIFLength
                                                    thumbsize = ifd_data;
                                                    break;
                                                case 0xA005: // InteroperabilityOffset
                                                    interoper = ifd_data;
                                                    break;
                                            }
                                        }
                                        if (ifd_format == 3 && ifd_numcomp == 1) {
                                            switch (ifd_tag) {
                                                case 0xA002: // ExifImageWidth
                                                    item->XRes = ifd_data;
                                                    break;
                                                case 0xA003: // ExifImageHeight
                                                    item->YRes = ifd_data;
                                                    break;
                                                case 0x9209: // Flash
                                                    item->Flash = ((ifd_data & 0x01) != 0x0);
                                                    break;
                                                case 0x112: // Orientation
                                                    item->Orientation = ifd_data;
                                                    break;
                                            }
                                        }
                                        if (ifd_format == 7) {
                                            if (ifd_tag == 0x927C) // MakerNote
                                            {
                                                makernote = ifd_data;
                                            }
                                        }
                                        if ((sizeary[ifd_format < 13 ? ifd_format : ifdf_undef] * ifd_numcomp) > 4) {
                                            if (ifd_format != ifdf_undef) {
                                            }
                                        }
                                        marker = markercnt = 0;
                                        ifd_numdir--;
                                        if (!ifd_numdir) {
                                            state = ExifState_ifdoffset;
                                        }
                                }
                                break;

                            case ExifState_ifdoffset:
                                if (markercnt == 4) {
                                    if (makernote) {
                                        skip = (makernote - offset) - 1;
                                        makernote = 0;
                                        currenttags = mn_canon_tags;
                                        state = ExifState_ifdnum;
                                    } else if (interoper) {
                                        skip = (interoper - offset) - 1;
                                        interoper = 0;
                                        currenttags = exif_itags;
                                        state = ExifState_ifdnum;
                                    } else if (nextifd) {
                                        skip = (nextifd - offset) - 1;
                                        nextifd = 0;
                                        thumbifd = B_BENDIAN_TO_HOST_INT32(marker);
                                        currenttags = exif_tags;
                                        state = ExifState_ifdnum;
                                    } else if (thumbifd) {
                                        skip = (thumbifd - offset) - 1;
                                        thumbifd = 0;
                                        currenttags = exif_tags;
                                        state = ExifState_ifdnum;
                                    } else if (thumbdata) {
                                        DEBUG("ExifParse: thumbdata %08lx\n", thumbdata);
                                        skip = (thumbdata - offset) - 1;
                                        thumbdata = 0;
                                        state = ExifState_thumbdata;
                                    } else {
                                        DEBUG("ExifParse: no thumbdata\n");
                                        state = 100;
                                    }
                                    DEBUG("ExifParse: offset %08lx skip %08lx marker %08lx\n", offset, skip, marker);
                                    marker = markercnt = 0;
                                }
                                break;

                            case ExifState_thumbdata:
                                if (!(thumbsize)) {
                                    DEBUG("ExifParse: got thumbnail data\n");
                                    return (1);
                                } else {
                                    memfh->Write((const void *) (buf + cnt), bytesread - cnt);
                                    thumbsize -= (bytesread - cnt);
                                    cnt = bytesread;
                                }
                                break;

                            default:
                                DEBUG("ExifParse: Unknown state\n");
                                return (0);
                        }
                    }
                }

                length -= bytesread;
            } else {
                return (0);
            }
        }
    }
    return (1);
}

//
//		exiftagname()
//

const char * exiftagname(struct exiftag taglist[], unsigned int tag) {
    uint32 cnt;

    for (cnt = 0; taglist[cnt].name; cnt++) {
        if (taglist[cnt].value == tag)
            return (taglist[cnt].name);
    }
    return (NULL);
}

const char stdIcon[] = {'I', 'D', 'X', 'Z', 0x05, 0x53, 0x00};
//
//		SetJPEGFileAttrs()
//
/*void SetJPEGFileAttrs(BFile *fh, PictureDesc *item)
{
        BNodeInfo* 	ni		= new BNodeInfo(fh);
        int32		width	= (item->Orientation==1 || item->Orientation==7) ? item->XRes : item->YRes;
        int32		height	= (item->Orientation==1 || item->Orientation==7) ? item->YRes : item->XRes;
        bool		flash	= item->Flash?true:false;
        bigtime_t	time	= mktime(gmtime((const time_t *)(&(item->Date))));

        fh->WriteAttr("GRAFX:Width", B_INT32_TYPE, 0, (const void *)&width, sizeof(int32));
        fh->WriteAttr("GRAFX:Height", B_INT32_TYPE, 0, (const void *)&height, sizeof(int32));
        fh->WriteAttr("EXIF:Flash", B_BOOL_TYPE, 0, (const void *)&flash, sizeof(bool));
        fh->WriteAttr("EXIF:Date", B_TIME_TYPE, 0, (const void *)&time, sizeof(time));
        switch (item->Type)
        {
                case picJPEG :	
                        ni->SetType("image/jpeg"); 
                        break;
                case picMovie :
                        ni->SetType("video/x-msvideo"); 
                        break;
                case picRaw :
                        ni->SetType("image/raw");
                default:
                        DEBUG("SetJPegFileAttrs: Picture type unknown\n");
        }*/
/*if (item->thumb)
{
        fh->WriteAttr("BEOS:D:STD_ICON", 'iICO', 0,(const void*)&stdIcon, sizeof(stdIcon));
        BBitmap* icon	= new BBitmap(BRect(0, 0, 256, 256), B_RGB32, true, false);
        BView* v		= new BView(icon->Bounds(), "temp", B_FOLLOW_NONE, 0);
        icon->AddChild(v);
        icon->Lock();
        v->DrawBitmap(item->thumb, icon->Bounds());
        icon->Unlock();
        icon->RemoveChild(v);
        delete v;
        fh-> WriteAttr("BEOS:V:STD_ICON", 'zICO', 0, (const void*)(icon->Bits()), icon->BitsLength());
        delete icon;
}*/
/*	delete ni;
}
 */

