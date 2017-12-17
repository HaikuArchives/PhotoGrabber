/* curently this file is included into ptp.c */
#include "PTPFunctions.h"

static inline void ptp_unpack_DI (uint16 byteorder, char* data, PTPDeviceInfo *di);
static inline void ptp_unpack_SI (uint16 byteorder, char* data, PTPStorageInfo *si);
static inline uint32 ptp_pack_OI (uint16 byteorder, PTPObjectInfo *oi, char** oidataptr);
static inline void ptp_unpack_OI (uint16 byteorder, char* data, PTPObjectInfo *oi);
static inline void ptp_unpack_DPV (uint16 byteorder, char* data, void** value, uint16 datatype);
static inline void ptp_unpack_DPD (uint16 byteorder, char* data, PTPDevicePropDesc *dpd);
static inline uint32 ptp_pack_DPV (uint16 byteorder, void* value, char** dpvptr, uint16 datatype);
static inline void ptp_unpack_EC (uint16 byteorder, char* data, PTPUSBEventContainer *ec);
static inline void ptp_unpack_Canon_FE (uint16 byteorder, char* data, PTPCANONFolderEntry *fe);
