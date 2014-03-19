#!/usr/bin/env bash

apt-get update
add-apt-repository ppa:pasgui/ppa
apt-get update
apt-get --yes --force-yes install lubuntu-core build-essential cmake git-core libboost-dev libboost-date-time-dev libboost-filesystem-dev libboost-graph-dev libboost-iostreams-dev libboost-program-options-dev libboost-regex-dev libboost-serialization-dev libboost-signals-dev libboost-system-dev libboost-thread-dev libqt4-dev libunittest++-dev libvtk5-dev libvtk5-qt4-dev libopencv-dev libusb-1.0-0-dev libcv-dev libhighgui-dev libdc1394-22 libdc1394-22-dev codeblocks
wget http://www.phidgets.com/downloads/libraries/libphidget.tar.gz
tar xzvf libphidget.tar.gz && cd libphidget*
./configure && make && sudo make install
cd ..
git clone https://github.com/rmsalinas/aruco.git
cd aruco/trunk/
mkdir build && cd build
cmake ..
make && sudo make install
