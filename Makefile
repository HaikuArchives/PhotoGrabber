## Haiku Generic Makefile v2.6 ##

## Fill in this file to specify the project being created, and the referenced
## Makefile-Engine will do all of the hard work for you. This handles any
## architecture of Haiku.

# The name of the binary.
NAME = PhotoGrabber
TARGET_DIR := $(shell pwd)/dist
PLUGIN_TARGET_DIR := $(TARGET_DIR)/plugins
ARCH = $(shell getarch)

# The type of binary, must be one of:
#	APP:	Application
#	SHARED:	Shared library or add-on
#	STATIC:	Static library archive
#	DRIVER: Kernel driver
TYPE = APP

# 	If you plan to use localization, specify the application's MIME signature.
APP_MIME_SIG = application/x-vnd-Fishpond.PhotoGrabber

#	The following lines tell Pe and Eddie where the SRCS, RDEFS, and RSRCS are
#	so that Pe and Eddie can fill them in for you.
#%{
# @src->@

#	Specify the source files to use. Full paths or paths relative to the
#	Makefile can be included. All files, regardless of directory, will have
#	their object files created in the common object directory. Note that this
#	means this Makefile will not work correctly if two source files with the
#	same name (source.c or source.cpp) are included from different directories.
#	Also note that spaces in folder names do not work well with this Makefile.
SRCS := framework/plugin_fw/PluginInterface.cpp \
	framework/extension_fw/ExtensionInterface.cpp \
	core_system/core_system.cpp \
	core_system/Camera.cpp \
	core_system/logger.cpp \
	interface/AboutWindow.cpp \
	interface/ConfigWindow.cpp \
	interface/PicItem.cpp \
	interface/MainWindow.cpp \
	interface/GridView.cpp \
	interface/StatusDock.cpp \
	interface/IconView.cpp

#	Specify the resource definition files to use. Full or relative paths can be
#	used.
RDEFS = $(NAME)-version.rdef \
		core_system/PhotoGrabber.rdef \
		core_system/DataAction.rdef

#	Specify the resource files to use. Full or relative paths can be used.
#	Both RDEFS and RSRCS can be utilized in the same Makefile.
RSRCS =

# End Pe/Eddie support.
# @<-src@
#%}

#	Specify libraries to link against.
#	There are two acceptable forms of library specifications:
#	-	if your library follows the naming pattern of libXXX.so or libXXX.a,
#		you can simply specify XXX for the library. (e.g. the entry for
#		"libtracker.so" would be "tracker")
#
#	-	for GCC-independent linking of standard C++ libraries, you can use
#		$(STDCPPLIBS) instead of the raw "stdc++[.r4] [supc++]" library names.
#
#	- 	if your library does not follow the standard library naming scheme,
#		you need to specify the path to the library and it's name.
#		(e.g. for mylib.a, specify "mylib.a" or "path/mylib.a")
LIBS = $(STDCPPLIBS) be translation

#	Specify additional paths to directories following the standard libXXX.so
#	or libXXX.a naming scheme. You can specify full paths or paths relative
#	to the Makefile. The paths included are not parsed recursively, so
#	include all of the paths where libraries must be found. Directories where
#	source files were specified are	automatically included.
LIBPATHS =

#	Additional paths to look for system headers. These use the form
#	"#include <header>". Directories that contain the files in SRCS are
#	NOT auto-included here.
SYSTEM_INCLUDE_PATHS =

#	Additional paths paths to look for local headers. These use the form
#	#include "header". Directories that contain the files in SRCS are
#	automatically included.
LOCAL_INCLUDE_PATHS = framework/extension_fw framework/plugin_fw core_system

#	Specify the level of optimization that you want. Specify either NONE (O0),
#	SOME (O1), FULL (O2), or leave blank (for the default optimization level).
OPTIMIZE =

# 	Specify the codes for languages you are going to support in this
# 	application. The default "en" one must be provided too. "make catkeys"
# 	will recreate only the "locales/en.catkeys" file. Use it as a template
# 	for creating catkeys for other languages. All localization files must be
# 	placed in the "locales" subdirectory.
LOCALES =

#	Specify all the preprocessor symbols to be defined. The symbols will not
#	have their values set automatically; you must supply the value (if any) to
#	use. For example, setting DEFINES to "DEBUG=1" will cause the compiler
#	option "-DDEBUG=1" to be used. Setting DEFINES to "DEBUG" would pass
#	"-DDEBUG" on the compiler's command line.
DEFINES =

#	Specify the warning level. Either NONE (suppress all warnings),
#	ALL (enable all warnings), or leave blank (enable default warnings).
WARNINGS =

#	With image symbols, stack crawls in the debugger are meaningful.
#	If set to "TRUE", symbols will be created.
SYMBOLS :=

#	Includes debug information, which allows the binary to be debugged easily.
#	If set to "TRUE", debug info will be created.
DEBUGGER :=

#	Specify any additional compiler flags to be used.
COMPILER_FLAGS =

#	Specify any additional linker flags to be used.
LINKER_FLAGS =

#	Specify the version of this binary. Example:
#		-app 3 4 0 d 0 -short 340 -long "340 "`echo -n -e '\302\251'`"1999 GNU GPL"
#	This may also be specified in a resource.
V_MAJOR = 2
V_MIDDLE = 3
V_MINOR = 4
V_BUILD = 1
APP_VERSION := $(V_MAJOR).$(V_MIDDLE).$(V_MINOR)-$(V_BUILD)

#	(Only used when "TYPE" is "DRIVER"). Specify the desired driver install
#	location in the /dev hierarchy. Example:
#		DRIVER_PATH = video/usb
#	will instruct the "driverinstall" rule to place a symlink to your driver's
#	binary in ~/add-ons/kernel/drivers/dev/video/usb, so that your driver will
#	appear at /dev/video/usb when loaded. The default is "misc".
DRIVER_PATH =

subsystem: $(TARGET_DIR)/$(NAME)
	$(MAKE) -C ./plugins/msac "PLUGIN_TARGET_DIR=$(PLUGIN_TARGET_DIR)" "DEBUGGER=$(DEBUGGER)"
	$(MAKE) -C ./plugins/msd "PLUGIN_TARGET_DIR=$(PLUGIN_TARGET_DIR)" "DEBUGGER=$(DEBUGGER)"
	$(MAKE) -C ./plugins/bdcpPTP "PLUGIN_TARGET_DIR=$(PLUGIN_TARGET_DIR)" "DEBUGGER=$(DEBUGGER)"
#	$(MAKE) -C ./plugins/ptp --eval "PLUGIN_TARGET_DIR=$(PLUGIN_TARGET_DIR)" $(MKFLAGS)

## Include the Makefile-Engine
DEVEL_DIRECTORY = \
	$(shell findpaths -e B_FIND_PATH_DEVELOP_DIRECTORY etc/makefile-engine)
include $(DEVEL_DIRECTORY)

clean: distclean pluginclean

distclean:
	-rm -rf $(PACKAGE_DIR)
	-rm ./PackageInfo_$(ARCH)
	-rm ./$(NAME)-version.rdef
	-rm $(TARGET_DIR)/$(NAME)

pluginclean:
	$(MAKE) -C ./plugins/msac clean
	$(MAKE) -C ./plugins/msd clean
	$(MAKE) -C ../bdcpPTP clean
	-rm $(PLUGIN_TARGET_DIR)/*


$(NAME)-version.rdef:
	echo "resource app_signature \"$(APP_MIME_SIG)\";" > $(NAME)-version.rdef
	echo "resource app_version {" > $(NAME)-version.rdef
	echo "     major = $(V_MAJOR), middle = $(V_MIDDLE), minor = $(V_MINOR)," >> $(NAME)-version.rdef
	echo "     variety = B_APPV_ALPHA, internal = $(V_BUILD)," >> $(NAME)-version.rdef
	echo "     short_info=\"$(NAME)\"," >> $(NAME)-version.rdef
	echo "	   long_info=\"$(NAME) ©Fishpond 2017\"" >> $(NAME)-version.rdef
	echo "};" >> $(NAME)-version.rdef

package: $(TARGET_DIR)/$(NAME) subsystem PackageInfo_$(ARCH)
	mkdir -p $(PACKAGE_DIR)/apps/$(NAME)/plugins
	mkdir -p $(PACKAGE_DIR)/data/deskbar/menu/Applications
	cp $(TARGET_DIR)/$(NAME) $(PACKAGE_DIR)/apps/$(NAME)/
	cp $(TARGET_DIR)/plugins/* $(PACKAGE_DIR)/apps/$(NAME)/plugins/
	-ln -s ../../../../apps/$(NAME)/$(NAME) $(PACKAGE_DIR)/data/deskbar/menu/Applications/$(NAME)
	package create -C $(PACKAGE_DIR)/ -i ./PackageInfo_$(ARCH) $(PACKAGE_PATH)
	copyattr -n BEOS:ICON $(TARGET_DIR)/$(NAME) $(PACKAGE_PATH)

PackageInfo_$(ARCH):
	cat ./PackageInfo | sed 's/$$VERSION/$(APP_VERSION)/' | sed 's/$$ARCH/$(ARCH)/' > ./PackageInfo_$(ARCH)

