#!/bin/bash

mkdir builds 
cd builds
mkdir build_android_release
cd build_android_release
konsole --noclose -e cmake ../../ -DANDROID=1 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#konsole --noclose -e make push run