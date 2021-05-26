#!/bin/bash

#mkdir cmake_bloat_tolinux
#cd cmake_bloat_tolinux
#
#konsole --noclose -e cmake ../ -UToAndroid -DToLinux=1 -DCMAKE_BUILD_TYPE=Debug
#konsole --noclose -e cmake --build .

#cd ../
mkdir cmake_bloat_toandroid
cd cmake_bloat_toandroid

konsole --noclose -e cmake ../ -DCMAKE_TOOLCHAIN_FILE=/home/marcfly/Documents/SqueLib/cmake_modules/SqueLib_Toolchain.cmake -DANDROID_VERSION=29 -DWITH_GLMATH=ON -DWITH_SOLOUD=ON -UToLinux -DToAndroid=1 -DCMAKE_BUILD_TYPE=Debug -DNDK_DEBUG=1
konsole --noclose -e cmake --build . -t SquePong -t uninstall -t push