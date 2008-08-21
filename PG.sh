## *****************************************************************
## * Copyright (c) 2004-2008,	Jan-Rixt Van Hoye				   *
## * All rights reserved.										   *
## * Distributed under the terms of the MIT License.               *
## *****************************************************************

## Buildfile for PhotoGrabber

# Cleaning the build system
make clean
cd interface
make clean
#cd ../plugins/msd
#make clean
cd ../plugins/ptp
make clean
cd ../..
rm -r build

# Building the files
make
cd interface
make
#cd ../plugins/msd
#make
cd ../plugins/ptp
make
cd ../..

# Creating target dirs
mkdir build
mkdir ./build/PhotoGrabber
mkdir ./build/PhotoGrabber/interface
mkdir ./build/PhotoGrabber/plugins
mkdir ./build/PhotoGrabber/Language
mkdir ./build/PhotoGrabber/Language/Dictionaries

# Moving bins over
mv ./obj.x86/PhotoGrabber ./build/PhotoGrabber/
mv ./interface/obj.x86/libPGInterface ./build/PhotoGrabber/interface/
#mv ./plugins/msd/obj.x86/libbdcp_msd ./build/PhotoGrabber/plugins/
mv ./plugins/ptp/obj.x86/libbdcp_ptp ./build/PhotoGrabber/plugins/

# Copy the translation files and documentation
cp ./translation/* ./build/PhotoGrabber/Language/Dictionaries/
cp ./doc/readme ./build/PhotoGrabber/

