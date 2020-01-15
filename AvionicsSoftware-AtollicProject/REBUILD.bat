@ECHO OFF
rd /s /q "build" > nul 2>&1
rd /s /q "bin" > nul 2>&1
mkdir build
cd build
D:\ProgramFiles\JetBrains\CLion_2019.3.2\bin\cmake\win\bin\cmake .. -G "CodeBlocks - MinGW Makefiles"
D:\ProgramFiles\MinGW\bin\mingw32-make -j8
cd ..