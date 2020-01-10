rm -r -fo .\build\ 2>&1 | out-null
rm -r -fo .\bin\ 2>&1 | out-null
mkdir build
cd .\build\
cmake .. -G "CodeBlocks - MinGW Makefiles"
make -j8
cd ..


