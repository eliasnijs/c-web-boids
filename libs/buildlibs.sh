#!/bin/bash


cd imgui/

CXX=g++
EMCXX=emcc
CXXFLAGS="-Wall"
EMCXXFLAGS="-Wall -I. -Ibackends"

SOURCES=("imgui.cpp" "imgui_draw.cpp" "imgui_widgets.cpp" "imgui_tables.cpp" "backends/imgui_impl_glfw.cpp" "backends/imgui_impl_opengl3.cpp")
OBJECTS=()
EM_OBJECTS=()
EM_LIBRARY="libimgui.a"

for src in "${SOURCES[@]}"; do
    OBJECTS+=("${src%.cpp}.o")
    EM_OBJECTS+=("${src%.cpp}.em.o")
done

build_all() {
    build_em_library
}

build_em_library() {
    for src in "${SOURCES[@]}"; do
        emcc $EMCXXFLAGS -c $src -o "${src%.cpp}.em.o"
    done
    emcc -r "${EM_OBJECTS[@]}" -o $EM_LIBRARY
}

clean() {
    rm -f "${OBJECTS[@]}" "${EM_OBJECTS[@]}" $EM_LIBRARY
}

case "$1" in
    all)
        build_all
        ;;
    clean)
        clean
        ;;
    *)
        echo "Usage: $0 {all|clean}"
        exit 1
        ;;
esac

