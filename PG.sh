## *****************************************************************
## * Copyright (c) 2004-2010, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## 	Buildfile for PhotoGrabber

#	Global Variables
VERSION=2.2
REVISION=114

# 	Cleaning the build system
make clean
cd interface
make clean
cd ../plugins/ptp
make clean
cd ../..
BUILDDIR="./build"
if [ -d "$BUILDDIR" ] ; then
	rm -r build
fi
# 	Creating target dirs
mkdir build
mkdir ./build/PhotoGrabber
mkdir ./build/PhotoGrabber/interface
mkdir ./build/PhotoGrabber/plugins
# 	Build the core
make
# 	Build the interface
cd interface
make
# 	Build the ptp plugin
cd ../plugins/ptp/libptp
make
cd ..
#cd ../plugins/ptp
make
cd ../
# 	Build the msd plugin
cd ./msd
make
cd ../..
# 	Make the package
cd build
zip -r "PhotoGrabber_V${VERSION}_R${REVISION}.zip" PhotoGrabber
cd ..

