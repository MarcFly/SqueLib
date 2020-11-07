#!/bin/bash

mkdir builds 
cd builds
mkdir build_android_release
cd build_android_release

#Building native apps following CNLohr system
#konsole --noclose -e cmake ../../ -DANDROID=1 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
#konsole --noclose -e make uninstall push cleanup

#Building native apps following taka-no-me system
#konsole --noclose -e cmake ../../ -DANDROID=1 -DCMAKE_BUILD_TYPE=Release -DANDROID_ABI="armeabi-v7a with NEON"
#konsole --noclose -e cmake --build .

#Building CNLohr translated to cmake system
konsole --noclose -e cmake ../../ -DANDROID=1 -DCMAKE_BUILD_TYPE=Release -DARM64=1
konsole --noclose -e cmake --build .