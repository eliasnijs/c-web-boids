#!/bin/sh

SOURCE="src/main.c"
# screenshot info
NAME="boids"
VERSION="00"

# includes and libs
INCS="-I libs/base -I libs/glad -I libs/imgui"
LIBS="-L libs/imgui -limgui -lm -lGL -lpthread -lglfw"

# flags
CPPFLAGS="-D_DEFAULT_SOURCE -D_BSD_SOURCE -D_POSIX_C_SOURCE=2 -DVERSION=\"${VERSION}\""
CFLAGS="-g -Wall -Wno-deprecated-declarations -Wno-write-strings -Wno-unused-function -O0 -fsanitize=address ${INCS} ${CPPFLAGS}"
LDFLAGS="${LIBS}"

# compiler and linker
CC="g++"

# building
echo "[build.sh]# building $NAME"
echo ${CC} $CFLAGS -o "build/$NAME" $SOURCE $LDFLAGS
${CC} $CFLAGS -o "build/$NAME" $SOURCE $LDFLAGS
echo "[build.sh]# finished building"
