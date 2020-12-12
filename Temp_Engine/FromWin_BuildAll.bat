mkdir cmake_bloat_towin & 
cd cmake_bloat_towin & 
cmake ../ -G "Unix Makefiles" -UToAndroid -UToWindows -DToLinux=1 
cmake --build . --config Release



cmd /k