## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
##,* 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber PTP plugin
# PTP lib
cd ./libptp
make clean
make
cd ..
# Cleaning the build system
make clean
# Build the ptp plugin
make

