rm -rf build
rm -rf bin
mkdir build
cd build || return
cmake .. -G "CodeBlocks - Unix Makefiles"
make -j8
cd ..

