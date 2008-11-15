## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber Mass storage plugin

# Cleaning the build system
#make clean
# Before building, check if the private headers are in place
PRIVATEDIR="./private"

if [ ! -d "$PRIVATEDIR" ] ; then
	mkdir private
	cd ./private
	svn checkout svn://svn.berlios.de/haiku/haiku/trunk/headers/private/storage storage
	svn checkout svn://svn.berlios.de/haiku/haiku/trunk/headers/private/shared shared
	cd ..
fi
# Build the ptp plugin
make
# Copy the binaries to the build directory
cp ./obj.x86/libbdcp_msd ../../build/PhotoGrabber/plugins/

