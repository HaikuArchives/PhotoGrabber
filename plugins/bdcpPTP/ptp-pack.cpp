/* curently this file is included into ptp.c */
#include "PTPFunctions.h"
#include <ByteOrder.h>
#include <string.h>
#include <stdlib.h>

static inline uint16 htod16p (uint16 byteorder, uint16 var)
{
	return ((byteorder==PTP_DL_LE)?B_HOST_TO_LENDIAN_INT16(var):B_HOST_TO_BENDIAN_INT16(var));
}

static inline uint32 htod32p (uint16 byteorder, uint32 var)
{
	return ((byteorder==PTP_DL_LE)?B_HOST_TO_LENDIAN_INT32(var):B_HOST_TO_BENDIAN_INT32(var));
}

static inline void htod16ap (uint16 byteorder, char *a, uint16 val)
{
	if (byteorder==PTP_DL_LE)
		val = B_HOST_TO_LENDIAN_INT16(*a); 
	else 
		val = B_HOST_TO_BENDIAN_INT16(*a);
}

static inline void htod32ap (uint16 byteorder, char *a, uint32 val)
{
	if (byteorder==PTP_DL_LE)
		val = B_HOST_TO_LENDIAN_INT32(*a); 
	else 
		val = B_HOST_TO_BENDIAN_INT32(*a);
}

static inline uint16 dtoh16p (uint16 byteorder, uint16 var)
{
	return ((byteorder==PTP_DL_LE)?B_LENDIAN_TO_HOST_INT16(var):B_BENDIAN_TO_HOST_INT16(var));
}

static inline uint32 dtoh32p (uint16 byteorder, uint32 var)
{
	return ((byteorder==PTP_DL_LE)?B_LENDIAN_TO_HOST_INT32(var):B_BENDIAN_TO_HOST_INT32(var));
}

static inline uint16 dtoh16ap (uint16 byteorder, char *a)
{
	return ((byteorder==PTP_DL_LE)?B_LENDIAN_TO_HOST_INT16(*a):B_BENDIAN_TO_HOST_INT16(*a));
}

static inline uint32 dtoh32ap (uint16 byteorder, char *a)
{
	return ((byteorder==PTP_DL_LE)?B_LENDIAN_TO_HOST_INT32(*a):B_BENDIAN_TO_HOST_INT32(*a));
}

#define htod8a(a,x)	*(uint8*)(a) = x
#define htod16a(a,x)	htod16ap(byteorder,a,x)
#define htod32a(a,x)	htod32ap(byteorder,a,x)
#define htod16(x)	htod16p(byteorder,x)
#define htod32(x)	htod32p(byteorder,x)

#define dtoh8a(x)	(*(uint8*)(x))
#define dtoh16a(a)	dtoh16ap(byteorder,a)
#define dtoh32a(a)	dtoh32ap(byteorder,a)
#define dtoh16(x)	dtoh16p(byteorder,x)
#define dtoh32(x)	dtoh32p(byteorder,x)


static inline char* ptp_unpack_string(uint16 byteorder, char* data, uint16 offset, uint8 *len)
{
	int i;
	char *string=NULL;

	*len=dtoh8a(&data[offset]);
	if (*len) {
		string=(char*)malloc(*len);
		memset(string, 0, *len);
		for (i=0;i<*len && i< PTP_MAXSTRLEN; i++) {
			string[i]=(char)dtoh16a(&data[offset+i*2+1]);
		}
		/* be paranoid! :( */
		string[*len-1]=0;
	}
	return (string);
}

static inline void ptp_pack_string(uint16 byteorder, char *string, char* data, uint16 offset, uint8 *len)
{
	int i;
	*len = (uint8)strlen(string);
	
	/* XXX: check strlen! */
	htod8a(&data[offset],*len+1);
	for (i=0;i<*len && i< PTP_MAXSTRLEN; i++) {
		htod16a(&data[offset+i*2+1],(uint16)string[i]);
	}
}

static inline uint32 ptp_unpack_uint32_array(uint16 byteorder, char* data, uint16 offset, uint32 **array)
{
	uint32 n, i=0;

	n=dtoh32a(&data[offset]);
	*array = (uint32*)malloc (n*sizeof(uint32));
	while (n>i) {
		(*array)[i]=dtoh32a(&data[offset+(sizeof(uint32)*(i+1))]);
		i++;
	}
	return n;
}

static inline uint32 ptp_unpack_uint16_array(uint16 byteorder, char* data, uint16 offset, uint16 **array)
{
	uint32 n, i=0;

	n=dtoh32a(&data[offset]);
	*array = (uint16*)malloc (n*sizeof(uint16));
	while (n>i) {
		(*array)[i]=dtoh16a(&data[offset+(sizeof(uint16)*(i+2))]);
		i++;
	}
	return n;
}

/* DeviceInfo pack/unpack */

#define PTP_di_StandardVersion		 0
#define PTP_di_VendorExtensionID	 2
#define PTP_di_VendorExtensionVersion	 6
#define PTP_di_VendorExtensionDesc	 8
#define PTP_di_FunctionalMode		 8
#define PTP_di_OperationsSupported	10

static inline void ptp_unpack_DI (uint16 byteorder, char* data, PTPDeviceInfoRaw *di)
{
	uint8 len;
	unsigned int totallen;
	
	di->StandardVersion = dtoh16a(&data[PTP_di_StandardVersion]);
	di->VendorExtensionID =
		dtoh32a(&data[PTP_di_VendorExtensionID]);
	di->VendorExtensionVersion =
		dtoh16a(&data[PTP_di_VendorExtensionVersion]);
	di->VendorExtensionDesc = 
		ptp_unpack_string(byteorder, data,
		PTP_di_VendorExtensionDesc, &len); 
	totallen=len*2+1;
	di->FunctionalMode = 
		dtoh16a(&data[PTP_di_FunctionalMode+totallen]);
	di->OperationsSupported_len = ptp_unpack_uint16_array(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&di->OperationsSupported);
	totallen=totallen+di->OperationsSupported_len*sizeof(uint16)+sizeof(uint32);
	di->EventsSupported_len = ptp_unpack_uint16_array(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&di->EventsSupported);
	totallen=totallen+di->EventsSupported_len*sizeof(uint16)+sizeof(uint32);
	di->DevicePropertiesSupported_len =
		ptp_unpack_uint16_array(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&di->DevicePropertiesSupported);
	totallen=totallen+di->DevicePropertiesSupported_len*sizeof(uint16)+sizeof(uint32);
	di->CaptureFormats_len = ptp_unpack_uint16_array(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&di->CaptureFormats);
	totallen=totallen+di->CaptureFormats_len*sizeof(uint16)+sizeof(uint32);
	di->ImageFormats_len = ptp_unpack_uint16_array(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&di->ImageFormats);
	totallen=totallen+di->ImageFormats_len*sizeof(uint16)+sizeof(uint32);
	di->Manufacturer = ptp_unpack_string(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&len);
	totallen+=len*2+1;
	di->Model = ptp_unpack_string(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&len);
	totallen+=len*2+1;
	di->DeviceVersion = ptp_unpack_string(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&len);
	totallen+=len*2+1;
	di->SerialNumber = ptp_unpack_string(byteorder, data,
		PTP_di_OperationsSupported+totallen,
		&len);
}
	
/* ObjectHandles array pack/unpack */

#define PTP_oh				 0

static inline void
ptp_unpack_OH (uint16 byteorder, char* data, PTPObjectHandles *oh)
{
	oh->n = ptp_unpack_uint32_array(byteorder, data, PTP_oh, &oh->Handler);
}

/* StorageInfo pack/unpack */

#define PTP_si_StorageType		 0
#define PTP_si_FilesystemType		 2
#define PTP_si_AccessCapability		 4
#define PTP_si_MaxCapability		 6
#define PTP_si_FreeSpaceInBytes		14
#define PTP_si_FreeSpaceInImages	22
#define PTP_si_StorageDescription	26

static inline void ptp_unpack_SI (uint16 byteorder, char* data, PTPStorageInfo *si)
{
	uint8 storagedescriptionlen;

	si->StorageType=dtoh16a(&data[PTP_si_StorageType]);
	si->FilesystemType=dtoh16a(&data[PTP_si_FilesystemType]);
	si->AccessCapability=dtoh16a(&data[PTP_si_AccessCapability]);
	/* XXX no dtoh64a !!! skiping next two */
	si->FreeSpaceInImages=dtoh32a(&data[PTP_si_FreeSpaceInImages]);
	si->StorageDescription=ptp_unpack_string(byteorder, data,
		PTP_si_StorageDescription, &storagedescriptionlen);
	si->VolumeLabel=ptp_unpack_string(byteorder, data,
		PTP_si_StorageDescription+storagedescriptionlen*2+1,
		&storagedescriptionlen);
}

/* ObjectInfo pack/unpack */

#define PTP_oi_StorageID		 0
#define PTP_oi_ObjectFormat		 4
#define PTP_oi_ProtectionStatus		 6
#define PTP_oi_ObjectCompressedSize	 8
#define PTP_oi_ThumbFormat		12
#define PTP_oi_ThumbCompressedSize	14
#define PTP_oi_ThumbPixWidth		18
#define PTP_oi_ThumbPixHeight		22
#define PTP_oi_ImagePixWidth		26
#define PTP_oi_ImagePixHeight		30
#define PTP_oi_ImageBitDepth		34
#define PTP_oi_ParentObject		38
#define PTP_oi_AssociationType		42
#define PTP_oi_AssociationDesc		44
#define PTP_oi_SequenceNumber		48
#define PTP_oi_filenamelen		52
#define PTP_oi_Filename			53

static inline uint32 ptp_pack_OI (uint16 byteorder, PTPObjectInfo *oi, char** oidataptr)
{
	char* oidata;
	uint8 filenamelen;
	uint8 capturedatelen=0;
	/* let's allocate some memory first; XXX i'm sure it's wrong */
	oidata=(char*)malloc(PTP_oi_Filename+(strlen(oi->Filename)+1)*2+4);
	/* the caller should free it after use! */
#if 0
	char *capture_date="20020101T010101"; /* XXX Fake date */
#endif
	memset (oidata, 0, (PTP_oi_Filename+(strlen(oi->Filename)+1)*2+4));
	htod32a(&oidata[PTP_oi_StorageID],oi->StorageID);
	htod16a(&oidata[PTP_oi_ObjectFormat],oi->ObjectFormat);
	htod16a(&oidata[PTP_oi_ProtectionStatus],oi->ProtectionStatus);
	htod32a(&oidata[PTP_oi_ObjectCompressedSize],oi->ObjectCompressedSize);
	htod16a(&oidata[PTP_oi_ThumbFormat],oi->ThumbFormat);
	htod32a(&oidata[PTP_oi_ThumbCompressedSize],oi->ThumbCompressedSize);
	htod32a(&oidata[PTP_oi_ThumbPixWidth],oi->ThumbPixWidth);
	htod32a(&oidata[PTP_oi_ThumbPixHeight],oi->ThumbPixHeight);
	htod32a(&oidata[PTP_oi_ImagePixWidth],oi->ImagePixWidth);
	htod32a(&oidata[PTP_oi_ImagePixHeight],oi->ImagePixHeight);
	htod32a(&oidata[PTP_oi_ImageBitDepth],oi->ImageBitDepth);
	htod32a(&oidata[PTP_oi_ParentObject],oi->ParentObject);
	htod16a(&oidata[PTP_oi_AssociationType],oi->AssociationType);
	htod32a(&oidata[PTP_oi_AssociationDesc],oi->AssociationDesc);
	htod32a(&oidata[PTP_oi_SequenceNumber],oi->SequenceNumber);
	
	ptp_pack_string(byteorder, oi->Filename, oidata, PTP_oi_filenamelen, &filenamelen);
/*
	filenamelen=(uint8_t)strlen(oi->Filename);
	htod8a(&req->data[PTP_oi_filenamelen],filenamelen+1);
	for (i=0;i<filenamelen && i< PTP_MAXSTRLEN; i++) {
		req->data[PTP_oi_Filename+i*2]=oi->Filename[i];
	}
*/
	/*
	 *XXX Fake date.
	 * for example Kodak sets Capture date on the basis of EXIF data.
	 * Spec says that this field is from perspective of Initiator.
	 */
#if 0	/* seems now we don't need any data packed in OI dataset... for now ;)*/
	capturedatelen=strlen(capture_date);
	htod8a(&data[PTP_oi_Filename+(filenamelen+1)*2],
		capturedatelen+1);
	for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
		data[PTP_oi_Filename+(i+filenamelen+1)*2+1]=capture_date[i];
	}
	htod8a(&data[PTP_oi_Filename+(filenamelen+capturedatelen+2)*2+1],
		capturedatelen+1);
	for (i=0;i<capturedatelen && i< PTP_MAXSTRLEN; i++) {
		data[PTP_oi_Filename+(i+filenamelen+capturedatelen+2)*2+2]=
		  capture_date[i];
	}
#endif
	/* XXX this function should return dataset length */
	
	*oidataptr=oidata;
	return (PTP_oi_Filename+(filenamelen+1)*2+(capturedatelen+1)*4);
}

static inline void ptp_unpack_OI (uint16 byteorder, char* data, PTPObjectInfo *oi)
{
	uint8 filenamelen;
	uint8 capturedatelen;
	char *capture_date;
	char tmp[16];
	struct tm tm;

	memset(&tm,0,sizeof(tm));

	oi->StorageID=dtoh32a(&data[PTP_oi_StorageID]);
	oi->ObjectFormat=dtoh16a(&data[PTP_oi_ObjectFormat]);
	oi->ProtectionStatus=dtoh16a(&data[PTP_oi_ProtectionStatus]);
	oi->ObjectCompressedSize=dtoh32a(&data[PTP_oi_ObjectCompressedSize]);
	oi->ThumbFormat=dtoh16a(&data[PTP_oi_ThumbFormat]);
	oi->ThumbCompressedSize=dtoh32a(&data[PTP_oi_ThumbCompressedSize]);
	oi->ThumbPixWidth=dtoh32a(&data[PTP_oi_ThumbPixWidth]);
	oi->ThumbPixHeight=dtoh32a(&data[PTP_oi_ThumbPixHeight]);
	oi->ImagePixWidth=dtoh32a(&data[PTP_oi_ImagePixWidth]);
	oi->ImagePixHeight=dtoh32a(&data[PTP_oi_ImagePixHeight]);
	oi->ImageBitDepth=dtoh32a(&data[PTP_oi_ImageBitDepth]);
	oi->ParentObject=dtoh32a(&data[PTP_oi_ParentObject]);
	oi->AssociationType=dtoh16a(&data[PTP_oi_AssociationType]);
	oi->AssociationDesc=dtoh32a(&data[PTP_oi_AssociationDesc]);
	oi->SequenceNumber=dtoh32a(&data[PTP_oi_SequenceNumber]);
	oi->Filename= ptp_unpack_string(byteorder, data, PTP_oi_filenamelen, &filenamelen);

	capture_date = ptp_unpack_string(byteorder, data,
		PTP_oi_filenamelen+filenamelen*2+1, &capturedatelen);
	/* subset of ISO 8601, without '.s' tenths of second and 
	 * time zone
	 */
	if (capturedatelen>15)
	{
		strncpy (tmp, capture_date, 4);
		tmp[4] = 0;
		tm.tm_year=atoi (tmp) - 1900;
		strncpy (tmp, capture_date + 4, 2);
		tmp[2] = 0;
		tm.tm_mon = atoi (tmp) - 1;
		strncpy (tmp, capture_date + 6, 2);
		tmp[2] = 0;
		tm.tm_mday = atoi (tmp);
		strncpy (tmp, capture_date + 9, 2);
		tmp[2] = 0;
		tm.tm_hour = atoi (tmp);
		strncpy (tmp, capture_date + 11, 2);
		tmp[2] = 0;
		tm.tm_min = atoi (tmp);
		strncpy (tmp, capture_date + 13, 2);
		tmp[2] = 0;
		tm.tm_sec = atoi (tmp);
		oi->CaptureDate=mktime (&tm);
	}
	free(capture_date);

	/* now it's modification date ;) */
	capture_date = ptp_unpack_string(byteorder, data,
		PTP_oi_filenamelen+filenamelen*2
		+capturedatelen*2+2,&capturedatelen);
	if (capturedatelen>15)
	{
		strncpy (tmp, capture_date, 4);
		tmp[4] = 0;
		tm.tm_year=atoi (tmp) - 1900;
		strncpy (tmp, capture_date + 4, 2);
		tmp[2] = 0;
		tm.tm_mon = atoi (tmp) - 1;
		strncpy (tmp, capture_date + 6, 2);
		tmp[2] = 0;
		tm.tm_mday = atoi (tmp);
		strncpy (tmp, capture_date + 9, 2);
		tmp[2] = 0;
		tm.tm_hour = atoi (tmp);
		strncpy (tmp, capture_date + 11, 2);
		tmp[2] = 0;
		tm.tm_min = atoi (tmp);
		strncpy (tmp, capture_date + 13, 2);
		tmp[2] = 0;
		tm.tm_sec = atoi (tmp);
		oi->ModificationDate=mktime (&tm);
	}
	free(capture_date);
}

/* Custom Type Value Assignement (without Length) macro frequently used below */
#define CTVAL(type,func,target)  {					\
		*target = malloc(sizeof(type));				\
		**(type **)target =					\
			func(data);\
}

static inline void ptp_unpack_DPV (uint16 byteorder, char* data, void** value, uint16 datatype)
{

	switch (datatype) 
	{
		case PTP_DTC_INT8:
			CTVAL(int8,dtoh8a,value);
			break;
		case PTP_DTC_UINT8:
			CTVAL(uint8,dtoh8a,value);
			break;
		case PTP_DTC_INT16:
			CTVAL(int16,dtoh16a,value);
			break;
		case PTP_DTC_UINT16:
			CTVAL(uint16,dtoh16a,value);
			break;
		case PTP_DTC_INT32:
			CTVAL(int32,dtoh32a,value);
			break;
		case PTP_DTC_UINT32:
			CTVAL(uint32,dtoh32a,value);
			break;
		/* XXX: other int types are unimplemented */
		/* XXX: int arrays are unimplemented also */
		case PTP_DTC_STR:
		{
			uint8 len;
			*((char **)value)=ptp_unpack_string(byteorder,data,0,&len);
			break;
		}
	}
}


/* Device Property pack/unpack */

#define PTP_dpd_DevicePropertyCode	0
#define PTP_dpd_DataType		2
#define PTP_dpd_GetSet			4
#define PTP_dpd_FactoryDefaultValue	5

/* Custom Type Value Assignement macro frequently used below */
#define CTVA(type,func,target)  {					\
		target = malloc(sizeof(type));				\
		*(type *)target =					\
			func(&data[PTP_dpd_FactoryDefaultValue+totallen]);\
			totallen+=sizeof(type);				\
}

/* Many Custom Types Vale Assignement macro frequently used below */

#define MCTVA(type,func,target,n) {					\
		uint16 i;						\
		for (i=0;i<n;i++) {					\
			target[i] = malloc(sizeof(type));		\
			*(type *)target[i] =				\
			func(&data[PTP_dpd_FactoryDefaultValue+totallen]);\
			totallen+=sizeof(type);				\
		}							\
}

static inline void ptp_unpack_DPD (uint16 byteorder, char* data, PTPDevicePropDesc *dpd)
{
	uint8 len;
	int totallen=0;

	dpd->DevicePropertyCode=dtoh16a(&data[PTP_dpd_DevicePropertyCode]);
	dpd->DataType=dtoh16a(&data[PTP_dpd_DataType]);
	dpd->GetSet=dtoh8a(&data[PTP_dpd_GetSet]);
	dpd->FactoryDefaultValue = NULL;
	dpd->CurrentValue = NULL;
	switch (dpd->DataType) {
		case PTP_DTC_INT8:
			CTVA(int8,dtoh8a,dpd->FactoryDefaultValue);
			CTVA(int8,dtoh8a,dpd->CurrentValue);
			break;
		case PTP_DTC_UINT8:
			CTVA(uint8,dtoh8a,dpd->FactoryDefaultValue);
			CTVA(uint8,dtoh8a,dpd->CurrentValue);
			break;
		case PTP_DTC_INT16:
			CTVA(int16,dtoh16a,dpd->FactoryDefaultValue);
			CTVA(int16,dtoh16a,dpd->CurrentValue);
			break;
		case PTP_DTC_UINT16:
			CTVA(uint16,dtoh16a,dpd->FactoryDefaultValue);
			CTVA(uint16,dtoh16a,dpd->CurrentValue);
			break;
		case PTP_DTC_INT32:
			CTVA(int32,dtoh32a,dpd->FactoryDefaultValue);
			CTVA(int32,dtoh32a,dpd->CurrentValue);
			break;
		case PTP_DTC_UINT32:
			CTVA(uint32,dtoh32a,dpd->FactoryDefaultValue);
			CTVA(uint32,dtoh32a,dpd->CurrentValue);
			break;
		/* XXX: other int types are unimplemented */
		/* XXX: int arrays are unimplemented also */
		case PTP_DTC_STR:
			dpd->FactoryDefaultValue = (void*)ptp_unpack_string
				(byteorder,data,PTP_dpd_FactoryDefaultValue,&len);
			totallen=len*2+1;
			dpd->CurrentValue = (void*)ptp_unpack_string
				(byteorder, data, PTP_dpd_FactoryDefaultValue + 
				totallen, &len);
			totallen+=len*2+1;
			break;
	}
	/* if totallen==0 then Data Type format is not supported by this
	code or the Data Type is a string (with two empty strings as
	values). In both cases Form Flag should be set to 0x00 and FORM is
	not present. */
	dpd->FormFlag=PTP_DPFF_None;
	if (totallen==0) return;

	dpd->FormFlag=dtoh8a(&data[PTP_dpd_FactoryDefaultValue+totallen]);
	totallen+=sizeof(uint8);
	switch (dpd->FormFlag) {
		case PTP_DPFF_Range:
		switch (dpd->DataType) {
			case PTP_DTC_INT8:
			CTVA(int8,dtoh8a,dpd->FORM.Range.MinimumValue);
			CTVA(int8,dtoh8a,dpd->FORM.Range.MaximumValue);
			CTVA(int8,dtoh8a,dpd->FORM.Range.StepSize);
			break;
			case PTP_DTC_UINT8:
			CTVA(uint8,dtoh8a,dpd->FORM.Range.MinimumValue);
			CTVA(uint8,dtoh8a,dpd->FORM.Range.MaximumValue);
			CTVA(uint8,dtoh8a,dpd->FORM.Range.StepSize);
			break;
			case PTP_DTC_INT16:
			CTVA(int16,dtoh16a,dpd->FORM.Range.MinimumValue);
			CTVA(int16,dtoh16a,dpd->FORM.Range.MaximumValue);
			CTVA(int16,dtoh16a,dpd->FORM.Range.StepSize);
			break;
			case PTP_DTC_UINT16:
			CTVA(uint16,dtoh16a,dpd->FORM.Range.MinimumValue);
			CTVA(uint16,dtoh16a,dpd->FORM.Range.MaximumValue);
			CTVA(uint16,dtoh16a,dpd->FORM.Range.StepSize);
			break;
			case PTP_DTC_INT32:
			CTVA(int32,dtoh32a,dpd->FORM.Range.MinimumValue);
			CTVA(int32,dtoh32a,dpd->FORM.Range.MaximumValue);
			CTVA(int32,dtoh32a,dpd->FORM.Range.StepSize);
			break;
			case PTP_DTC_UINT32:
			CTVA(uint32,dtoh32a,dpd->FORM.Range.MinimumValue);
			CTVA(uint32,dtoh32a,dpd->FORM.Range.MaximumValue);
			CTVA(uint32,dtoh32a,dpd->FORM.Range.StepSize);
			break;
		/* XXX: other int types are unimplemented */
		/* XXX: int arrays are unimplemented also */
		/* XXX: does it make any sense: "a range of strings"? */
		}
		break;
		case PTP_DPFF_Enumeration:
#define N	dpd->FORM.Enum.NumberOfValues
		N = dtoh16a(&data[PTP_dpd_FactoryDefaultValue+totallen]);
		totallen+=sizeof(uint16);
		dpd->FORM.Enum.SupportedValue = (void**)malloc(N*sizeof(void *));
		switch (dpd->DataType) {
			case PTP_DTC_INT8:
			MCTVA(int8,dtoh8a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_UINT8:
			MCTVA(uint8,dtoh8a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_INT16:
			MCTVA(int16,dtoh16a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_UINT16:
			MCTVA(uint16,dtoh16a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_INT32:
			MCTVA(int32,dtoh16a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_UINT32:
			MCTVA(uint32,dtoh16a,dpd->FORM.Enum.SupportedValue,N);
			break;
			case PTP_DTC_STR:
			{
			int i;
			for(i=0;i<N;i++)
			{
				dpd->FORM.Enum.SupportedValue[i]= (void*)
					ptp_unpack_string
					(byteorder,data,PTP_dpd_FactoryDefaultValue
					+totallen,&len);
				totallen+=len*2+1;
			}
			}
			break;
		}
	}
}

static inline uint32 ptp_pack_DPV (uint16 byteorder, void* value, char** dpvptr, uint16 datatype)
{
	char* dpv=NULL;
	uint32 size=0;

	switch (datatype) {
		case PTP_DTC_INT8:
			size=sizeof(int8);
			dpv=new char(size);
			htod8a(dpv,*(int8*)value);
			break;
		case PTP_DTC_UINT8:
			size=sizeof(uint8);
			dpv=new char(size);
			htod8a(dpv,*(uint8*)value);
			break;
		case PTP_DTC_INT16:
			size=sizeof(int16);
			dpv=new char(size);
			htod16a(dpv,*(int16*)value);
			break;
		case PTP_DTC_UINT16:
			size=sizeof(uint16);
			dpv=new char(size);
			htod16a(dpv,*(uint16*)value);
			break;
		case PTP_DTC_INT32:
			size=sizeof(int32);
			dpv=new char(size);
			htod32a(dpv,*(int32*)value);
			break;
		case PTP_DTC_UINT32:
			size=sizeof(uint32);
			dpv=new char(size);
			htod32a(dpv,*(uint32*)value);
			break;
		/* XXX: other int types are unimplemented */
		/* XXX: int arrays are unimplemented also */
		case PTP_DTC_STR:
		{
			uint8 len;
			size=strlen((char*)value)*2+3;
			dpv=new char(size);
			memset(dpv,0,size);
			ptp_pack_string(byteorder, (char *)value, dpv, 0, &len);
		}
		break;
	}
	*dpvptr=dpv;
	return size;
}


/*
    PTP USB Event container unpack
    Copyright (c) 2003 Nikolai Kopanygin
*/

#define PTP_ec_Length		0
#define PTP_ec_Type		4
#define PTP_ec_Code		6
#define PTP_ec_TransId		8
#define PTP_ec_Param1		12
#define PTP_ec_Param2		16
#define PTP_ec_Param3		20

static inline void ptp_unpack_EC (uint16 byteorder, char* data, PTPUSBEventContainer *ec)
{
	if (data==NULL)
		return;
	ec->Length=dtoh32a(&data[PTP_ec_Length]);
	ec->Type=dtoh16a(&data[PTP_ec_Type]);
	ec->Code=dtoh16a(&data[PTP_ec_Code]);
	ec->TransactionID=dtoh32a(&data[PTP_ec_TransId]);
	if (ec->Length>=(PTP_ec_Param1+4))
		ec->Param1=dtoh32a(&data[PTP_ec_Param1]);
	else
		ec->Param1=0;
	if (ec->Length>=(PTP_ec_Param2+4))
		ec->Param2=dtoh32a(&data[PTP_ec_Param2]);
	else
		ec->Param2=0;
	if (ec->Length>=(PTP_ec_Param3+4))
		ec->Param3=dtoh32a(&data[PTP_ec_Param3]);
	else
		ec->Param3=0;
}

/*
    PTP Canon Folder Entry unpack
    Copyright (c) 2003 Nikolai Kopanygin
*/
#define PTP_cfe_ObjectHandle		0
#define PTP_cfe_ObjectFormatCode	4
#define PTP_cfe_Flags			6
#define PTP_cfe_ObjectSize		7
#define PTP_cfe_Time			11
#define PTP_cfe_Filename		15

static inline void
ptp_unpack_Canon_FE (uint16 byteorder, char* data, PTPCANONFolderEntry *fe)
{
	int i;
	if (data==NULL)
		return;
	fe->ObjectHandle=dtoh32a(&data[PTP_cfe_ObjectHandle]);
	fe->ObjectFormatCode=dtoh16a(&data[PTP_cfe_ObjectFormatCode]);
	fe->Flags=dtoh8a(&data[PTP_cfe_Flags]);
	fe->ObjectSize=dtoh32a(&data[PTP_cfe_ObjectSize]);
	fe->Time=(time_t)dtoh32a(&data[PTP_cfe_Time]);
	for (i=0; i<PTP_CANON_FilenameBufferLen; i++)
	fe->Filename[i]=(char)dtoh8a(&data[PTP_cfe_Filename+i]);
}


