rm -rf build/
cmake -B build -S . -DENABLE_GUI=0 -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build build --config Release --target all