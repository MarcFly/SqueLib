#!/bin/bash

mkdir builds
cd builds
mkdir build_linux_release
cd build_linux_release
konsole --noclose -e cmake ../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
konsole --noclose -e make
