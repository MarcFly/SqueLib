#!/bin/bash

mkdir cmake_bloat_tolinux
cd cmake_bloat_tolinux

konsole --noclose -e cmake ../ -UToAndroid -DToLinux=1 -DWITH_IMGUI=ON -DCMAKE_BUILD_TYPE=Release
konsole --noclose -e cmake --build .

cd ../
mkdir cmake_bloat_toandroid
cd cmake_bloat_toandroid

konsole --noclose -e cmake ../ -DCMAKE_TOOLCHAIN_FILE=/home/marcfly/Documents/SqueLib/toolchains/SqueLib.toolchain -DANDROID_VERSION=29 -DWITH_IMGUI=ON -UToLinux -DToAndroid=1 -DCMAKE_BUILD_TYPE=Release
konsole --noclose -e cmake --build . -t InputViz -t uninstall -t push