#!/bin/bash

mkdir cmake_bloat_tolinux
cd cmake_bloat_tolinux

konsole --noclose -e cmake ../ -UToAndroid -DToLinux=1 -DWITH_IMGUI=ON -DCMAKE_BUILD_TYPE=Debug
konsole --noclose -e cmake --build .

cd ../
mkdir cmake_bloat_toandroid
cd cmake_bloat_toandroid
konsole --noclose -e cmake ../ -DCMAKE_TOOLCHAIN_FILE=/home/marcfly/Documents/SqueLib/toolchains/SqueLib.toolchain -DANDROID_VERSION=29 -UToLinux -DToAndroid=1 -DWITH_IMGUI=ON -DCMAKE_BUILD_TYPE=Debug
konsole --noclose -e cmake --build .
konsole --noclose -e make uninstall push