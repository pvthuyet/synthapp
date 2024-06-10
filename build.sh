OUT=$(pwd)/out
OUT_GUI=$(pwd)/out_gui

if [ "$1" = "clean" ]; then
    echo "Remove out folder: $OUT"
    rm -rf $OUT
    mkdir $OUT

    rm -rf $OUT_GUI
    mkdir $OUT_GUI
    echo "******************************"
fi

# build console
echo "********* Console app building ..."
cmake -B $OUT -S . -DCMAKE_BUILD_TYPE:STRING=Release
cmake --build $OUT --config Release --target synthapp --clean-first

# build gui
echo "********* Gui app building ..."
cmake -B $OUT_GUI -S . -DCMAKE_BUILD_TYPE:STRING=Release -DSHOW_GUI=1
cmake --build $OUT_GUI --config Release --target synthapp --clean-first

# copy to bin
cp -f $OUT/synthapp_artefacts/Release/synthapp ./bin/synthapp
cp -f $OUT_GUI/synthapp_artefacts/Release/synthapp ./bin/synthapp_gui