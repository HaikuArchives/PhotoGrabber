/* ExifTag.h */

const char * 		exiftagname(struct exiftag taglist[], unsigned int tag);

struct exiftag
	{
	unsigned int	value;
	const char		*name;
	};

static struct exiftag exif_tags[] =
	{ 
	{0x100, "ImageWidth"},
	{0x101, "ImageLength"},
	{0x102, "BitsPerSample"},
	{0x103, "Compression"},
	{0x106, "PhotometricInterpretation"},
	{0x10A, "FillOrder"},
	{0x10D, "DocumentName"},
	{0x10E, "ImageDescription"},
	{0x10F, "Make"},
	{0x110, "Model"},
	{0x111, "StripOffsets"},
	{0x112, "Orientation"},
	{0x115, "SamplesPerPixel"},
	{0x116, "RowsPerStrip"},
	{0x117, "StripByteCounts"},
	{0x11A, "XResolution"},
	{0x11B, "YResolution"},
	{0x11C, "PlanarConfiguration"},
	{0x128, "ResolutionUnit"},
	{0x12D, "TransferFunction"},
	{0x131, "Software"},
	{0x132, "DateTime"},
	{0x13B, "Artist"},
	{0x13E, "WhitePoint"},
	{0x13F, "PrimaryChromaticities"},
	{0x156, "TransferRange"},
	{0x200, "JPEGProc"},
	{0x201, "JPEGInterchangeFormat"},
	{0x202, "JPEGInterchangeFormatLength"},
	{0x211, "YCbCrCoefficients"},
	{0x212, "YCbCrSubSampling"},
	{0x213, "YCbCrPositioning"},
	{0x214, "ReferenceBlackWhite"},
	{0x828D, "CFARepeatPatternDim"},
	{0x828E, "CFAPattern"},
	{0x828F, "BatteryLevel"},
	{0x8298, "Copyright"},
	{0x829A, "ExposureTime"},
	{0x829D, "FNumber"},
	{0x83BB, "IPTC/NAA"},
	{0x8769, "ExifOffset"},
	{0x8773, "InterColorProfile"},
	{0x8822, "ExposureProgram"},
	{0x8824, "SpectralSensitivity"},
	{0x8825, "GPSInfo"},
	{0x8827, "ISOSpeedRatings"},
	{0x8828, "OECF"},
	{0x9000, "ExifVersion"},
	{0x9003, "DateTimeOriginal"},
	{0x9004, "DateTimeDigitized"},
	{0x9101, "ComponentsConfiguration"},
	{0x9102, "CompressedBitsPerPixel"},
	{0x9201, "ShutterSpeedValue"},
	{0x9202, "ApertureValue"},
	{0x9203, "BrightnessValue"},
	{0x9204, "ExposureBiasValue"},
	{0x9205, "MaxApertureValue"},
	{0x9206, "SubjectDistance"},
	{0x9207, "MeteringMode"},
	{0x9208, "LightSource"},
	{0x9209, "Flash"},
	{0x920A, "FocalLength"},
	{0x9214, "SubjectArea"},
	{0x927C, "MakerNote"},
	{0x9286, "UserComment"},
	{0x9290, "SubSecTime"},
	{0x9291, "SubSecTimeOriginal"},
	{0x9292, "SubSecTimeDigitized"},
	{0xA000, "FlashPixVersion"},
	{0xA001, "ColorSpace"},
	{0xA002, "ExifImageWidth"},
	{0xA003, "ExifImageLength"},
	{0xA004, "RelatedSoundFile"},
	{0xA005, "InteroperabilityOffset"},
	{0xA20B, "FlashEnergy"},				// 0x920B in TIFF/EP 
	{0xA20C, "SpatialFrequencyResponse"},	// 0x920C  - - 
	{0xA20E, "FocalPlaneXResolution"},		// 0x920E  - - 
	{0xA20F, "FocalPlaneYResolution"},		// 0x920F  - - 
	{0xA210, "FocalPlaneResolutionUnit"},	// 0x9210  - - 
	{0xA214, "SubjectLocation"},			// 0x9214  - - 
	{0xA215, "ExposureIndex"},				// 0x9215  - - 
	{0xA217, "SensingMethod"},				// 0x9217  - - 
	{0xA300, "FileSource"},
	{0xA301, "SceneType"},
	{0xA302, "NewCFAPatterm"},
	{0xA401, "CustomRendered"},
	{0xA402, "ExposureMode"},
	{0xA403, "WhiteBalance"},
	{0xA404, "DigitalZoomRatio"},
	{0xA405, "FocalLenghtIn35mmFilm"},
	{0xA406, "SceneCaptureType"},
	{0xA407, "GainControl"},
	{0xA408, "Contrast"},
	{0xA409, "Saturation"},
	{0xA40A, "Sharpness"},
	{0xA40B, "DeviceSettingDescription"},
	{0xA40C, "SubjectDistanceRange"},
	{0xA420, "ImageUniqueID"},
	{0, 0}
}; 

static struct exiftag exif_itags[] = { 
	{0x1, "InteroperabilityIndex"},
	{0x2, "InteroperabilityVersion"},
	{0x1000, "RelatedImageFileFormat"},
	{0x1001, "RelatedImageWidth"},
	{0x1002, "RelatedImageLength"},
	{0, 0}
};

static struct exiftag mn_canon_tags[] = { 
	{0x1, "PowerShot_ImageQuality"},
	{0x2, "PowerShot_FocalLength"},
	{0x6, "PowerShot_StitchAssist"},
	{0x7, "PowerShot_FirmwareVersion"},
	{0, 0}
};

static struct exiftag exif_formats[] = { 
	{1 , "unsigned byte"},
	{2 , "string"},
	{3 , "unsigned short"},
	{4 , "unsigned long"},
	{5 , "unsigned rational"},
	{6 , "signed byte"},
	{7 , "undefined"},
	{8 , "signed short"},
	{9 , "signed long"},
	{10, "signed rational"},
	{11, "single float"},
	{12, "double float"},
	{0, 0}
};


