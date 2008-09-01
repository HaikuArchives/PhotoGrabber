## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber PTP plugin

# Cleaning the build system
#make clean
# Build the ptp plugin
make
# Copy the binaries to the build directory
cp ./obj.x86/libbdcp_ptp ../../build/PhotoGrabber/plugins/

