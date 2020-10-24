#!/bin/bash

mkdir builds 
cd builds
mkdir build_linux_debug
cd build_linux_debug
konsole --noclose -e cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
konsole --noclose -e make

