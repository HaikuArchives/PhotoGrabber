## *****************************************************************
## * Copyright (c) 2004-2008, Jan-Rixt Van Hoye aka'Jixt'		   *
## * 							and Luc Schrijvers aka 'Begasus'   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber Interface

# Build the interface
make
# Copy the binaries to the build directory
cp ./obj.x86/libPGInterface ../build/PhotoGrabber/interface/

