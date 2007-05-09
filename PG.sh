# buildfile for PhotoGrabber

# cleaning the build system
make clean
cd interface
make clean
cd ../plugins/msd
make clean
cd ../ptp
make clean
cd ../..
# rm -r build

# building the files
make
cd interface
make
cd ../plugins/msd
make
cd ../ptp
make
cd ../..


# creating target dirs
mkdir build
mkdir ./build/PhotoGrabber
mkdir ./build/PhotoGrabber/interface
mkdir ./build/PhotoGrabber/plugins
mkdir ./build/PhotoGrabber/Language
mkdir ./build/PhotoGrabber/Language/Dictionaries


# moving bins over
# ZETA
mv PhotoGrabber ./build/PhotoGrabber/
mv ./interface/libPGInterface ./build/PhotoGrabber/interface/
mv ./plugins/msd/libbdcp_msd ./build/PhotoGrabber/plugins/
mv ./plugins/ptp/libbdcp_ptp ./build/PhotoGrabber/plugins/

# BeOS/Haiku
mv ./obj.x86/PhotoGrabber ./build/PhotoGrabber/
mv ./interface/obj.x86/libPGInterface ./build/PhotoGrabber/interface/
mv ./plugins/msd/obj.x86/libbdcp_msd ./build/PhotoGrabber/plugins/
mv ./plugins/ptp/obj.x86/libbdcp_ptp ./build/PhotoGrabber/plugins/

cp ./translation/* ./build/PhotoGrabber/Language/Dictionaries/
cp ./doc/readme ./build/PhotoGrabber/

make clean
cd interface
make clean
cd ../plugins/msd
make clean
cd ../ptp
make clean
cd ../..
