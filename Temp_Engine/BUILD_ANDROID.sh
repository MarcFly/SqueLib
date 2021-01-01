#!/bin/bash

mkdir cmake_bloat
cd cmake_bloat

#Building CNLohr translated to cmake system
konsole --noclose -e cmake ../ -DCMAKE_VERBOSE_MAKEFILE=ON -DUSE_IMGUI=1 -UToLinux -DToAndroid=1 -DCMAKE_BUILD_TYPE=Release | tee BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt

konsole --noclose -e make uninstall push 