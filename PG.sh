## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## 	Buildfile for PhotoGrabber

#	Global Variables
VERSION=2.2
REVISION=$(svn info 2> /dev/null | grep 'Revision:' | cut -f 2 -d ' ')

# 	Cleaning the build system
make clean
cd interface
make clean
cd ../plugins/ptp
make clean
cd ../..
rm -r build

# 	Build the core
make
# 	Build the interface
cd interface
make
# 	Build the ptp plugin
cd ../plugins/ptp
make
cd ../
# 	Build the msd plugin
cd ./msd
PRIVATEDIR="./private"
if [ ! -d "$PRIVATEDIR" ] ; then
	mkdir private
	cd ./private
	svn checkout svn://svn.berlios.de/haiku/haiku/trunk/headers/private/storage storage
	svn checkout svn://svn.berlios.de/haiku/haiku/trunk/headers/private/shared shared
	cd ..
fi
make
cd ../..
# 	Creating target dirs
mkdir build
mkdir ./build/PhotoGrabber
mkdir ./build/PhotoGrabber/interface
mkdir ./build/PhotoGrabber/plugins

# 	Copy the binaries to the build directory
cp ./obj.x86/PhotoGrabber ./build/PhotoGrabber/
cp ./interface/obj.x86/libPGInterface ./build/PhotoGrabber/interface/
cp ./plugins/ptp/obj.x86/libbdcp_ptp ./build/PhotoGrabber/plugins/
cp ./plugins/msd/obj.x86/libbdcp_msd ./build/PhotoGrabber/plugins/

# 	Make the package
cd build
zip -r "PhotoGrabber_V${VERSION}_R${REVISION}.zip" PhotoGrabber
cd ..

