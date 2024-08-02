#!/bin/sh

ARCHIVE_DIR="archives"
mkdir -p $ARCHIVE_DIR

IMGUI_DIR="imgui"
IMGUI_CXX=emcc
IMGUI_CXXFLAGS="-Wall -I. -Ibackends"
IMGUI_SOURCES=("imgui.cpp" "imgui_draw.cpp" "imgui_widgets.cpp" "imgui_tables.cpp" "backends/imgui_impl_glfw.cpp" "backends/imgui_impl_opengl3.cpp")

STB_DIR="stb"
STB_CXX=emcc
STB_CXXFLAGS="-Wall -I."
STB_SOURCES=("stb_image.cpp")


build() {
    local DIR=$1
    local CXX=$2
    local CXXFLAGS=$3
    shift 3
    local SOURCES=("$@")

    cd $DIR || exit 1

    OBJECTS=()
    LIBRARY="lib${DIR}.a"

    echo "building $LIBRARY... "

    for src in "${SOURCES[@]}"; do
	    OBJECTS+=("${src%.cpp}.o")
    done

    for src in "${SOURCES[@]}"; do
	    $CXX $CXXFLAGS -c $src -o "${src%.cpp}.o"
    done


    ar rcs "../$ARCHIVE_DIR/$LIBRARY" "${OBJECTS[@]}"

    rm -f "${OBJECTS[@]}"

    cd .. || exit 1
}

case "$1" in
    all)
        build $IMGUI_DIR $IMGUI_CXX "$IMGUI_CXXFLAGS" "${IMGUI_SOURCES[@]}"

        echo '#define STB_IMAGE_IMPLEMENTATION' > "$STB_DIR/stb_image.cpp"
        echo '#include "stb_image.h"' >> "$STB_DIR/stb_image.cpp"
        build $STB_DIR $STB_CXX "$STB_CXXFLAGS" "${STB_SOURCES[@]}"
	rm -f "$STB_DIR/stb_image.cpp"
        ;;
    clean)
	echo "cleaning..."
	rm -rf $ARCHIVE_DIR
        ;;
    *)
        echo "Usage: $0 {all|clean}"
        exit 1
        ;;
esac

