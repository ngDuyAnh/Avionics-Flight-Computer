rm -rf build
rm -rf bin
mkdir build
cd build || return
cmake ..
make -j8


