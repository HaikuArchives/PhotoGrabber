## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber

# Cleaning the build system
make clean
cd interface
make clean
cd ../plugins/ptp
make clean
cd ../..
rm -r build

# Build the core
make
# Build the interface
cd interface
make
# Build the ptp plugin
cd ../plugins/ptp
make
cd ../..

# Creating target dirs
mkdir build
mkdir ./build/PhotoGrabber
mkdir ./build/PhotoGrabber/interface
mkdir ./build/PhotoGrabber/plugins

# Copy the binaries to the build directory
cp ./obj.x86/PhotoGrabber ./build/PhotoGrabber/
cp ./interface/obj.x86/libPGInterface ./build/PhotoGrabber/interface/
cp ./plugins/ptp/obj.x86/libbdcp_ptp ./build/PhotoGrabber/plugins/

# Make the package
cd build
tar -czvf PhotoGrabber.tgz PhotoGrabber
cd ..

