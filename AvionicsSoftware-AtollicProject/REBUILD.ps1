rm -r -fo build 2>&1 | out-null
rm -r -fo bin 2>&1 | out-null
mkdir build
cd build
D:\ProgramFiles\JetBrains\CLion_2019.3.2\bin\cmake\win\bin\cmake .. -G "CodeBlocks - MinGW Makefiles"
D:\ProgramFiles\MinGW\bin\mingw32-make -j8
cd ..