export LD_LIBRARY_PATH=/usr/lib/i386-linux-gnu

echo "***************** 32 bit building ********************"
cmake -DCMAKE_TOOLCHAIN_FILE=./cmake/toolchain-i386-linux-gnu.cmake -DBUILD_X86=ON -B outx86 -S .
cmake --build outx86

echo
echo "***************** 64 bit building ********************"
cmake -B outx64 -S .
cmake --build outx64