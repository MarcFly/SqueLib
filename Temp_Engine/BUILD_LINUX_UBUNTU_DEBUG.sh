#!/bin/bash

mkdir -p cmake_bloat_linux_debug
cd cmake_bloat_linux_debug

konsole --noclose -e cmake ../ -UToAndroid -UToWindows -DToLinux=1 -DUSE_IMGUI=1 -DCMAKE_BUILD_TYPE=Debug | tee BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt

