BUILD=$(pwd)/build/
if [ "$1" = "clean" ]; then
    echo "Remove build folder: $BUILD"
    rm -rf $BUILD
    echo "******************************"
fi

cmake -B $BUILD -S . -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build $BUILD --config Release --target all --clean-first