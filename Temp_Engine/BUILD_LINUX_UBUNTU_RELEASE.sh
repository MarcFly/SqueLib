#!/bin/bash

mkdir -p cmake_bloat
cd cmake_bloat

konsole --noclose -e cmake ../ -UToAndroid -UToWindows -DToLinux=1 -DUSE_IMGUI=1 -DCMAKE_BUILD_TYPE=Release | tee BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt