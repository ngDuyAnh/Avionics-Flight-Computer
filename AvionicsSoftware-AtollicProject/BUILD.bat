@ECHO OFF
rd /s /q "build" > nul 2>&1
rd /s /q "bin" > nul 2>&1
mkdir build
cd build
cmake .. -G "CodeBlocks - MinGW Makefiles"
mingw32-make -j8
cd ..
