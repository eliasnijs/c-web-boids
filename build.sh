#!/bin/sh
mkdir -p build

SOURCE="src/main.cpp"
# screenshot info
VERSION="00"

# PLATFORM SPECIFIC
if [ "$1" = "web" ]; then
    CC="em++"
    CFLAGS="${CFLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -DPLATFORM_WEB"
    LIBS="-L libs/imgui/build/ -limgui"
    NAME="boids.html"
else
    CC="g++"
    LIBS="-L libs/imgui/ -limgui"
    NAME="boids"
fi

# includes and libs
INCS="${INCS} -I libs/base -I libs/glad -I libs/imgui"
LIBS="${LIBS} -lm -lGL -lpthread -lglfw"

# flags
CPPFLAGS="${CPPFLAGS} -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="${CFLAGS} -g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-writable-strings -Wno-unused-function -O0 ${INCS} ${CPPFLAGS}"


# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
${CC} $CFLAGS -o "build/$NAME" $SOURCE $LIBS
echo "[build.sh]# finished building"
