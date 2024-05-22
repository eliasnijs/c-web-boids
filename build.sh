#!/bin/sh
mkdir -p build

SOURCE="src/main.cpp"
NAME="boids.html"
VERSION="00"

# includes and libs
INCS="${INCS} -I libs/base -I libs/glad -I libs/imgui"
LIBS="-lm -lGL -lpthread -lglfw -L libs/imgui/build/ -limgui"

# flags
EMFLAGS="-s USE_GLFW=3 -s FULL_ES3=1 -s USE_WEBGL2=1 -DPLATFORM_WEB"
CPPFLAGS="${CPPFLAGS} -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="${CFLAGS} -g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-writable-strings -Wno-unused-function -O0 ${INCS} ${CPPFLAGS}"

# files
FILES="${FILES} --preload-file src/view/shaders/boids.vertex.glsl"
FILES="${FILES} --preload-file src/view/shaders/boids.fragment.glsl"

# compiler
CC="em++"

# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS ${EMFLAGS} ${FILES} -o "build/$NAME" $SOURCE $LIBS
${CC} $CFLAGS ${EMFLAGS} ${FILES} -o "build/$NAME" $SOURCE $LIBS
echo "[build.sh]# finished building"
