konsole --noclose -e cmake . -DToAndroid=1 -DToLinux=1 -DCMAKE_BUILD_TYPE=Release | tee BuildLog.txt
konsole --noclose -e cmake --build . | tee -a BuildLog.txt