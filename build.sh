#!/bin/sh

rm -r build
mkdir -p build

SOURCE="src/main.cpp"
NAME="boids.html"
VERSION="00"

# files
SET_FRONTEND_TEMPLATE="--shell-file site/index.html"
FILES="${FILES} --preload-file src/view/shaders/boids.vertex.glsl"
FILES="${FILES} --preload-file src/view/shaders/boids.fragment.glsl"

# includes and libs
INCS="${INCS} -I libs/base -I libs/glad -I libs/imgui"
LIBS="-lm -lGL -lpthread -lglfw -L libs/imgui/ -limgui"

# flags
EMFLAGS="-s USE_GLFW=3 -s FULL_ES3=1 -s USE_WEBGL2=1 -DPLATFORM_WEB"
CPPFLAGS="${CPPFLAGS} -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="${CFLAGS} -g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-writable-strings -Wno-unused-function -O3 ${INCS} ${CPPFLAGS}"

# compiler
CC="em++"

# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS ${EMFLAGS} ${FILES} -o "build/$NAME" $SOURCE $LIBS $SET_FRONTEND_TEMPLATE
${CC} $CFLAGS ${EMFLAGS} ${FILES} -o "build/$NAME" $SOURCE $LIBS $SET_FRONTEND_TEMPLATE
echo "[build.sh]# finished building"

