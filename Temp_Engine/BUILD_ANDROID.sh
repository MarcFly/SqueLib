#!/bin/bash

mkdir builds 
cd builds
mkdir build_android_release
cd build_android_release

#Building CNLohr translated to cmake system
konsole --noclose -e cmake ../../ -DANDROID=1 -DCMAKE_BUILD_TYPE=Release -UARM32 -DARM64=1 | tee BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt

konsole --noclose -e cmake ../../ -DANDROID=1 -DCMAKE_BUILD_TYPE=Release -UARM64 -DARM32=1 | tee -a BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt

konsole --noclose -e make uninstall push 