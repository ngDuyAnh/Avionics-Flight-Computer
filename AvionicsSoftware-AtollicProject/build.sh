rm -rf build
rm -rf bin
mkdir build
cd build || return
cmake .. -G "CodeBlocks - MinGW Makefiles"
make -j8
cd ..

