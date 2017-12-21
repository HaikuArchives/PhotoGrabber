NAME= bdcpPTP
TYPE= SHARED
TARGET_DIR := $(PLUGIN_TARGET_DIR)
#%{
# @src->@
SRCS=	bdcpCapture.cpp \
	bdcpConfig.cpp \
	bdcpFunctions.cpp \
	BitmapView.cpp \
	exifparse.cpp \
	PictureList.cpp \
	PTPFunctions.cpp

RDEFS= bdcpPTP.rdef
# @<-src@
#%}
APP_VERSION= -app 0 9 0 b 1 -short "bdcpPTP" -long "bdcpPTP© Fishpond 2014-2017"
LIBS= root be translation $(STDCPPLIBS) device tracker
#LIBS= root be translation stdc++ device
LIBPATHS=
SYSTEM_INCLUDE_PATHS= /boot/develop/headers/be \
	/boot/system/develop/headers/cpp \
	/boot/system/develop/headers/posix

LOCAL_INCLUDE_PATHS= .
OPTIMIZE=FULL
#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES=
#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = suppress all warnings
#	ALL = enable all warnings
WARNINGS=ALL
# Build with debugging symbols if set to TRUE
SYMBOLS=TRUE
COMPILER_FLAGS=
LINKER_FLAGS=
DEBUGGER=
#
#
MENU_LINKS=
APP_MENU=


## Include the Makefile-Engine
DEVEL_DIRECTORY := \
	$(shell findpaths -r "makefile_engine" B_FIND_PATH_DEVELOP_DIRECTORY)
include $(DEVEL_DIRECTORY)/etc/makefile-engine
